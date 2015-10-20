/*
Copyright (C) 2011 by Ladislav Hrabcak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "base/base.h"
#include "base/app.h"
#include "benchmark.h"

int main(int argc, char* argv[])
{
	try {
		benchmark myapp;
		
		for(int i = 1; i < argc; ++i) {
			if(stricmp(argv[i], "--debug-ctx") == 0) {
				base::cfg().use_debug_context = true;
				base::cfg().use_debug_sync = false;
			}
			else if(stricmp(argv[i], "--debug-ctx-sync") == 0) {
				base::cfg().use_debug_context = true;
				base::cfg().use_debug_sync = true;
			}
			else if(stricmp(argv[i], "--use-pinned-memory") == 0) {
				base::cfg().use_pinned_memory = true;
			}
			else if(stricmp(argv[i], "--use-nvidia-fast-download") == 0) {
				base::cfg().use_nvidia_fast_download = true;
			}
			else if(stricmp(argv[i], "--use-async-readback") == 0) {
				base::cfg().use_async_readback = true;
			}
			else {
				printf(
					"--debug-ctx  Enable debug contex and GL_ARB_debug_output / GL_AMD_debug_output\n"
					"--debug-ctx-sync  Same as --debug-ctx but enable GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB\n"
					"--use-pinned-memory Will use GL_AMD_pinned_memory extension\n"
					"--use-nvidia-fast-download Will call glReadpixel into GPU mem buffer and then glCopyBufferSubData to drv mem\n"
					"--use-sync-readback\n\n");
				return -1;
				
			}

		}

		base::run_app_win(&myapp, false);
	}
	catch(const base::exception &e) {
		std::cout << e.text();
		return -1;
	}
	return 0;
}
