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
#include "Windows.h"
#include "Shellapi.h"

LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs)
{
    int retval;
    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, NULL, 0);
    if (!SUCCEEDED(retval))
        return NULL;

    LPWSTR lpWideCharStr = (LPWSTR)malloc(retval * sizeof(WCHAR));
    if (lpWideCharStr == NULL)
        return NULL;

    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
    if (!SUCCEEDED(retval))
    {
        free(lpWideCharStr);
        return NULL;
    }

    int numArgs;
    LPWSTR* args;
    args = CommandLineToArgvW(lpWideCharStr, &numArgs);
    free(lpWideCharStr);
    if (args == NULL)
        return NULL;

    int storage = numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++i)
    {
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, NULL, 0, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(args);
            return NULL;
        }

        storage += retval;
    }

    LPSTR* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
    if (result == NULL)
    {
        LocalFree(args);
        return NULL;
    }

    int bufLen = storage - numArgs * sizeof(LPSTR);
    LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++i)
    {
        assert(bufLen > 0);
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(result);
            LocalFree(args);
            return NULL;
        }

        result[i] = buffer;
        buffer += retval;
        bufLen -= retval;
    }

    LocalFree(args);

    *pNumArgs = numArgs;
    return result;
}

int WINAPI WinMain(
    HINSTANCE /*hInstance*/,
    HINSTANCE /*hPrevInstance*/,
    LPSTR lpCmdLine,
    int /*nCmdShow*/)
{
    int argc;
    LPSTR * const argv = CommandLineToArgvA(GetCommandLineA(), &argc);

	try {
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
            else if (
                   stricmp(argv[i], "--test0") == 0
                || stricmp(argv[i], "--test1") == 0
                || stricmp(argv[i], "--test2") == 0
                || stricmp(argv[i], "--test3") == 0) {
                base::cfg().test = argv[i][6] - '0';
            }
            else if (
                   stricmp(argv[i], "--tex-mode0") == 0
                || stricmp(argv[i], "--tex-mode1") == 0
                || stricmp(argv[i], "--tex-mode2") == 0
                || stricmp(argv[i], "--tex-mode3") == 0) {
                base::cfg().tex_mode = argv[i][10] - '0';
            }
            else if (
                   stricmp(argv[i], "--mesh-size0") == 0
                || stricmp(argv[i], "--mesh-size1") == 0
                || stricmp(argv[i], "--mesh-size2") == 0
                || stricmp(argv[i], "--mesh-size3") == 0
                || stricmp(argv[i], "--mesh-size4") == 0) {
                base::cfg().mesh_size = argv[i][11] - '0';
            }
            else if (
                   stricmp(argv[i], "--tex-freq0") == 0
                || stricmp(argv[i], "--tex-freq1") == 0
                || stricmp(argv[i], "--tex-freq2") == 0
                || stricmp(argv[i], "--tex-freq3") == 0) {
                base::cfg().tex_freq = (argv[i][10] - '0') - 1;
            }
            else if (stricmp(argv[i], "--one-mesh") == 0) {
                base::cfg().one_mesh = true;
            }
            else if (stricmp(argv[i], "--use-vbo") == 0) {
                base::cfg().use_vbo = true;
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

        if (base::cfg().tex_mode == 1 && (base::cfg().test == 2 || base::cfg().test == 3)) {
            printf("test mode and texture mode combination is invalid!");
            return -1;
        }

        benchmark myapp;

		base::run_app_win(&myapp, false);
	}
	catch(const base::exception &e) {
		std::cout << e.text();
		return -1;
	}
	return 0;
}
