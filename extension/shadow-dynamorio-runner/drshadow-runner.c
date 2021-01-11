/*
 * Copyright (c) 2008, Katsuhiro Suzuki.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the Katsuhiro Suzuki nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* original from: http://www.katsuster.net/?arg_date=20080724 (Japanese)
 * adapted for x86_64
 */

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

int parent_main(void)
{
    pid_t child;
    int status, sig;
    int result;
    while (1)
    {
        child = waitpid(-1, &status, WUNTRACED | WCONTINUED);
        if (child == -1)
        {
            perror("waitpid");
            return 1;
        }
        sig = 0;
        if (WIFEXITED(status))
        {
            break;
        }
        else if (WIFSIGNALED(status))
        {
            break;
        }
        else if (WIFSTOPPED(status))
        {
            if (WSTOPSIG(status) != SIGTRAP)
            {
                sig = WSTOPSIG(status);
            }
        }
        else if (WIFCONTINUED(status))
        {
        }
        else
        {
            return 1;
        }
        result = ptrace(PTRACE_SYSCALL, child, NULL, sig);
        if (result == -1)
        {
            perror("PTRACE(PTRACE_SYSCALL, ...)");
            return 1;
        }
    }

    return 0;
}

int child_main(const char *filename, char *argv[])
{
    int result;
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    result = execvp(filename, argv);
    if (result)
    {
        perror("execvp");
        return result;
    }
    printf("[ERROR] NOT RUN SHADOW\n");
    return 0;
}

int main(int argc, char *argv[])
{
    const char resolved_path[MAXPATHLEN];
    const char *filename;
    pid_t pid;
    int result;
    realpath(argv[0], resolved_path);
    if (argc < 2)
    {
        printf("usage: \n%s execfile [options]\n", argv[0]);
        return 0;
    }
    filename = argv[1];
    pid = fork();
    if (pid)
    {
        printf("[RUNNER]:[%5d] shadow started\n", pid);
        result = parent_main();
    }
    else
    {
        result = child_main(filename, &argv[1]);
    }

    return result;
}
