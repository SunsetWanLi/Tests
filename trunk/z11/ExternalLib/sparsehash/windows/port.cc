/* Copyright (c) 2007, Google Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---
 * Author: Craig Silverstein
 */

#include <sparsehash/internal/sparseconfig.h>
#ifndef WIN32
# error You should only be including windows/port.cc in a windows environment!
#endif

#include "sparsehash_config.h"
#include <stdarg.h>    // for va_list, va_start, va_end
#include "port.h"

// Calls the windows _vsnprintf, but always NUL-terminate.
int __snprintf(char *str, size_t size, const char *format, ...) {
  if (size == 0)        // not even room for a \0?
    return -1;          // not what C99 says to do, but what windows does
  str[size-1] = '\0';
  va_list ap;
  va_start(ap, format);
  const int r = _vsnprintf(str, size-1, format, ap);
  va_end(ap);
  return r;
}

std::string TmpFile(const char* basename) {
  char tmppath_buffer[1024];
  int tmppath_len = GetTempPathA(sizeof(tmppath_buffer), tmppath_buffer);
  if (tmppath_len <= 0 || tmppath_len >= sizeof(tmppath_buffer)) {
    return basename;           // an error, so just bail on tmppath
  }
  __snprintf(tmppath_buffer + tmppath_len, sizeof(tmppath_buffer) - tmppath_len,
           "\\%s", basename);
  return tmppath_buffer;
}
