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

void set_thread_affinity(unsigned int cpu_num)
{
	SetThreadAffinityMask(GetCurrentThread(), (DWORD_PTR)1 << cpu_num);
}