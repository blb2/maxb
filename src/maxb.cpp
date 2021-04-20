/*
 * Copyright (C) 2018-2021, Brian Brice. All rights reserved.
 *
 * This file is part of maxb.
 *
 * maxb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * maxb is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with maxb.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "platform.h"

struct sender_data {
	std::chrono::high_resolution_clock::time_point reftime;
	std::chrono::seconds duration;

	asio::ip::udp::socket*   p_socket;
	asio::ip::udp::endpoint* p_endpoint;

	const uint8_t* p_buf;
	int buf_size;
	int pkt_size;

	std::thread       thread;
	std::promise<int> nsends;
};

static bool parse_addr(const char* str, asio::ip::udp::endpoint& endpoint)
{
	asio::error_code ec;
	endpoint.address(asio::ip::address::from_string(str, ec));
	return !ec;
}

static bool parse_int(const char* str, int& value)
{
	try {
		value = std::stoi(str);
		return true;
	} catch (...) {
		return false;
	}
}

static void send(asio::ip::udp::socket& sock, const uint8_t* p_buf, int buf_size, int pkt_size, asio::ip::udp::endpoint* p_endpoint)
{
	const uint8_t* p_buf_end = p_buf + buf_size;
	asio::error_code ec;

	while (p_buf != p_buf_end) {
		size_t nbytes_send = std::min<size_t>(p_buf_end - p_buf, pkt_size);
		size_t nbytes_sent = (p_endpoint ? sock.send_to(asio::buffer(p_buf, nbytes_send), *p_endpoint, 0, ec) :
		                                   sock.send(asio::buffer(p_buf, nbytes_send), 0, ec));

		if (ec) {
			fprintf(stderr, "send error: %s\n", ec.message().c_str());
			break;
		}

		if (nbytes_sent != nbytes_send) {
			fprintf(stderr, "send incomplete\n");
			if (nbytes_sent == 0)
				break;
		}

		p_buf += nbytes_sent;
	}
}

static void send(sender_data* p_sender)
{
	int nsends;

	for (nsends = 0; std::chrono::high_resolution_clock::now() - p_sender->reftime < p_sender->duration; nsends++)
		send(*p_sender->p_socket, p_sender->p_buf, p_sender->buf_size, p_sender->pkt_size, p_sender->p_endpoint);

	p_sender->nsends.set_value(nsends);
}

int main(int argc, char* argv[])
{
	const int buf_size  = 1024 * 1024; // 1 MiB
	const int pkt_size  = 8000;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s ip time\n", argv[0]);
		return EXIT_FAILURE;
	} 

	asio::ip::udp::endpoint endpoint;
	if (!parse_addr(argv[1], endpoint)) {
		fprintf(stderr, "invalid address: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	int time_sec;
	if (!parse_int(argv[2], time_sec) || time_sec < 1) {
		fprintf(stderr, "invalid time: %s\n", argv[2]);
		return EXIT_FAILURE;
	}

	std::unique_ptr<uint8_t[]> p_buf = std::make_unique<uint8_t[]>(buf_size);
	for (size_t i = 0, n = buf_size; i != n; i++)
		p_buf[i] = 'A' + (i % 26);

	asio::io_service io;
	asio::io_service::work* p_work = new asio::io_service::work(io);

	std::vector<std::thread> io_threads;
	for (unsigned int i = 0, n = std::thread::hardware_concurrency(); i != n; i++)
		io_threads.emplace_back([i, &io]() { set_thread_affinity(i); io.run(); });

	std::random_device random_device;
	std::default_random_engine random_engine(random_device());
	std::uniform_int_distribution<uint16_t> random_dist(1024 + 1);

	asio::ip::udp::socket sock(io);
	endpoint.port(random_dist(random_engine));

	asio::ip::udp::endpoint* p_endpoint = nullptr;
	if (!endpoint.address().is_loopback()) {
		asio::error_code ec;
		sock.connect(endpoint, ec);
		assert(!ec);
	} else {
		sock.open(asio::ip::udp::v4());
		p_endpoint = &endpoint;
	}

	std::vector<sender_data> senders(std::thread::hardware_concurrency());
	for (auto& sender : senders) {
		sender.duration   = std::chrono::seconds(time_sec);
		sender.p_socket   = &sock;
		sender.p_endpoint = p_endpoint;
		sender.p_buf      = p_buf.get();
		sender.buf_size   = buf_size;
		sender.pkt_size   = pkt_size;
	}

	auto beg_time = std::chrono::high_resolution_clock::now();

	for (size_t i = 0, n = senders.size(); i != n; i++) {
		sender_data* p_sender = &senders[i];
		p_sender->reftime = beg_time;
		p_sender->thread  = std::thread([i, p_sender]() { set_thread_affinity((int)i); send(p_sender); });
	}

	int nsends = 0;
	for (auto& sender : senders) {
		nsends += sender.nsends.get_future().get();
		sender.thread.join();
	}

	auto   end_time = std::chrono::high_resolution_clock::now();
	double elapsed  = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - beg_time).count();

	delete p_work;
	for (auto& thread : io_threads)
		thread.join();

	printf("%f Mbps over %f seconds\n", ((8.0 * nsends * buf_size) / elapsed) / (1000.0 * 1000.0), elapsed);
	return 0;
}
