/*
 * Copyright(C) 2018 Brian Brice
 * 
 * This file is part of maxb.
 * 
 * maxb is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * maxb is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with maxb.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

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

int main(int argc, char* argv[])
{
	const int buf_size  = 1024 * 1024; // 1 MiB
	const int pkt_size  = 8000;
	const int max_sends = 100;

	if (argc != 2 && argc != 3) {
		fprintf(stderr, "Usage: %s ip [time]\n", argv[0]);
		return EXIT_FAILURE;
	} 

	asio::ip::udp::endpoint endpoint;
	if (!parse_addr(argv[1], endpoint)) {
		fprintf(stderr, "invalid address: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	int time_sec = 0;
	if (argc == 3 && !parse_int(argv[2], time_sec)) {
		fprintf(stderr, "invalid time: %s\n", argv[2]);
		return EXIT_FAILURE;
	}

	std::unique_ptr<uint8_t[]> p_buf = std::make_unique<uint8_t[]>(buf_size);
	for (size_t i = 0, n = buf_size; i != n; i++)
		p_buf[i] = 'A' + (i % 26);

	asio::io_service io;
	asio::io_service::work* p_work = new asio::io_service::work(io);

	std::vector<std::thread> threads;
	for (unsigned int i = 0, n = std::thread::hardware_concurrency(); i != n; i++)
		threads.emplace_back([&io]() { io.run(); });

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

	std::promise<bool> completion_promise;
	std::future<bool>  completion = completion_promise.get_future();

	auto beg_time = std::chrono::high_resolution_clock::now();

	int nsends;
	if (time_sec == 0) {
		for (nsends = 1; nsends != max_sends; nsends++)
			send(sock, p_buf.get(), buf_size, pkt_size, p_endpoint);
	} else {
		for (nsends = 1; std::chrono::high_resolution_clock::now() - beg_time < std::chrono::seconds(time_sec); nsends++)
			send(sock, p_buf.get(), buf_size, pkt_size, p_endpoint);
	}

	send(sock, p_buf.get(), buf_size, pkt_size, p_endpoint);
	completion_promise.set_value(true);
	completion.wait();

	auto   end_time = std::chrono::high_resolution_clock::now();
	double elapsed  = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - beg_time).count();

	delete p_work;
	for (auto& thread : threads)
		thread.join();

	printf("%f Mbps over %f seconds\n", ((8.0 * nsends * buf_size) / elapsed) / (1000.0 * 1000.0), elapsed);
	return 0;
}
