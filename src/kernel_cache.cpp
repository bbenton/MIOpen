/* ************************************************************************
 * Copyright 2015 Vratis, Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ************************************************************************ */


#include <mlopen/kernel_cache.hpp>

#include <iostream>
#include <iterator>

namespace mlopen {

OCLKernel KernelCache::getKernel(const std::string& algorithm,
										const std::string& network_config) {

	std::pair<std::string, std::string> key = std::make_pair(algorithm, network_config);
#ifndef NDEBUG
	std::cout << "key: " << key.first <<" "<< key.second<< std::endl;
#endif

	auto kernel_iterator = kernel_map.find(key);
	if (kernel_iterator != kernel_map.end())
	{
#ifndef NDEBUG
		printf("kernel found\n");
#endif
		return kernel_iterator->second;
	}
	// TODO: Where should the default kernel be?
	else // return default kernel
	{
		printf("looking for default kernel (does not exist)\n");
		exit(0);
	}
}

OCLKernel KernelCache::getKernel(cl_command_queue &queue,
										const std::string& algorithm,
										const std::string& network_config,
                                        const std::string& program_name,
                                        const std::string& kernel_name,
										const std::vector<size_t>& vld,
										const std::vector<size_t>& vgd,
                                        std::string params)
{

    if (params.length() > 0)
    {
        // Ensure only one space after the -cl-std.
        // >1 space can cause an Apple compiler bug. See clSPARSE issue #141.
        if (params.at(0) != ' ') params = " " + params;
    }

	std::pair<std::string, std::string> key = std::make_pair(algorithm, network_config);
#ifndef NDEBUG
    std::cout << "key: " << key.first << ',' << key.second << std::endl;
#endif

    auto kernel_iterator = kernel_map.find(key);
    if (kernel_iterator != kernel_map.end())
    {
#ifndef NDEBUG
		printf("kernel found\n");
#endif
        return kernel_iterator->second;
    }
    else //build program and compile the kernel;
    {
#ifndef NDEBUG
		printf("kernel not found\n");
#endif
		
		OCLKernel kernel(CreateKernel(GetContext(queue), GetDevice(queue), kernel_name, program_name, params), vld, vgd);
        if (!network_config.empty()) kernel_map[key] = kernel;
        return kernel;
    }
}

mlopenStatus_t KernelCache::getProgram(cl_program &program,
		cl_command_queue &queue,
                                         const std::string& program_name,
                                         const std::string& params)
{
	mlopenStatus_t status;

 	status = CLHelper::LoadProgramFromSource(program, queue, program_name);
	if(status != mlopenStatusSuccess) {
		return status;
	}

	status = CLHelper::BuildProgram(program, queue, params);
	if(status != mlopenStatusSuccess) {
		return status;
	}

	return mlopenStatusSuccess;
}

KernelCache::KernelCache()
{}

}
