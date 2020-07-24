## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
if (NOT CMAKE_INSTALL_PREFIX OR
    CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/../../../Install")
endif()
file(GLOB_RECURSE CURL_7.70.0_SRCS ${CMAKE_CURRENT_SOURCE_DIR} *.c *.h)


## using add_custom_command

add_custom_target(curl_7.70.0 ALL
  DEPENDS ${CMAKE_INSTALL_PREFIX}/curl_7.70.0/lib/libcurl.so.4.6.0
)

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/curl_7.70.0/lib/libcurl.so.4.6.0
  COMMAND gcc -shared  -fPIC -DPIC  .libs/libcurl_la-altsvc.o .libs/libcurl_la-amigaos.o .libs/libcurl_la-asyn-ares.o .libs/libcurl_la-asyn-thread.o .libs/libcurl_la-base64.o .libs/libcurl_la-conncache.o .libs/libcurl_la-connect.o .libs/libcurl_la-content_encoding.o .libs/libcurl_la-cookie.o .libs/libcurl_la-curl_addrinfo.o .libs/libcurl_la-curl_ctype.o .libs/libcurl_la-curl_des.o .libs/libcurl_la-curl_endian.o .libs/libcurl_la-curl_fnmatch.o .libs/libcurl_la-curl_get_line.o .libs/libcurl_la-curl_gethostname.o .libs/libcurl_la-curl_gssapi.o .libs/libcurl_la-curl_memrchr.o .libs/libcurl_la-curl_multibyte.o .libs/libcurl_la-curl_ntlm_core.o .libs/libcurl_la-curl_ntlm_wb.o .libs/libcurl_la-curl_path.o .libs/libcurl_la-curl_range.o .libs/libcurl_la-curl_rtmp.o .libs/libcurl_la-curl_sasl.o .libs/libcurl_la-curl_sspi.o .libs/libcurl_la-curl_threads.o .libs/libcurl_la-dict.o .libs/libcurl_la-dotdot.o .libs/libcurl_la-easy.o .libs/libcurl_la-escape.o .libs/libcurl_la-file.o .libs/libcurl_la-fileinfo.o .libs/libcurl_la-formdata.o .libs/libcurl_la-ftp.o .libs/libcurl_la-url.o .libs/libcurl_la-ftplistparser.o .libs/libcurl_la-getenv.o .libs/libcurl_la-getinfo.o .libs/libcurl_la-gopher.o .libs/libcurl_la-hash.o .libs/libcurl_la-hmac.o .libs/libcurl_la-hostasyn.o .libs/libcurl_la-hostcheck.o .libs/libcurl_la-hostip.o .libs/libcurl_la-hostip4.o .libs/libcurl_la-hostip6.o .libs/libcurl_la-hostsyn.o .libs/libcurl_la-http.o .libs/libcurl_la-http2.o .libs/libcurl_la-http_chunks.o .libs/libcurl_la-http_digest.o .libs/libcurl_la-http_negotiate.o .libs/libcurl_la-http_ntlm.o .libs/libcurl_la-http_proxy.o .libs/libcurl_la-idn_win32.o .libs/libcurl_la-if2ip.o .libs/libcurl_la-imap.o .libs/libcurl_la-inet_ntop.o .libs/libcurl_la-inet_pton.o .libs/libcurl_la-krb5.o .libs/libcurl_la-ldap.o .libs/libcurl_la-llist.o .libs/libcurl_la-md4.o .libs/libcurl_la-md5.o .libs/libcurl_la-memdebug.o .libs/libcurl_la-mime.o .libs/libcurl_la-mprintf.o .libs/libcurl_la-mqtt.o .libs/libcurl_la-multi.o .libs/libcurl_la-netrc.o .libs/libcurl_la-non-ascii.o .libs/libcurl_la-nonblock.o .libs/libcurl_la-openldap.o .libs/libcurl_la-parsedate.o .libs/libcurl_la-pingpong.o .libs/libcurl_la-pop3.o .libs/libcurl_la-progress.o .libs/libcurl_la-psl.o .libs/libcurl_la-doh.o .libs/libcurl_la-rand.o .libs/libcurl_la-rename.o .libs/libcurl_la-rtsp.o .libs/libcurl_la-security.o .libs/libcurl_la-select.o .libs/libcurl_la-sendf.o .libs/libcurl_la-setopt.o .libs/libcurl_la-sha256.o .libs/libcurl_la-share.o .libs/libcurl_la-slist.o .libs/libcurl_la-smb.o .libs/libcurl_la-smtp.o .libs/libcurl_la-socketpair.o .libs/libcurl_la-socks.o .libs/libcurl_la-socks_gssapi.o .libs/libcurl_la-socks_sspi.o .libs/libcurl_la-speedcheck.o .libs/libcurl_la-splay.o .libs/libcurl_la-strcase.o .libs/libcurl_la-strdup.o .libs/libcurl_la-strerror.o .libs/libcurl_la-strtok.o .libs/libcurl_la-strtoofft.o .libs/libcurl_la-system_win32.o .libs/libcurl_la-telnet.o .libs/libcurl_la-tftp.o .libs/libcurl_la-timeval.o .libs/libcurl_la-transfer.o .libs/libcurl_la-urlapi.o .libs/libcurl_la-version.o .libs/libcurl_la-warnless.o .libs/libcurl_la-wildcard.o .libs/libcurl_la-x509asn1.o vauth/.libs/libcurl_la-cleartext.o vauth/.libs/libcurl_la-cram.o vauth/.libs/libcurl_la-digest.o vauth/.libs/libcurl_la-digest_sspi.o vauth/.libs/libcurl_la-krb5_gssapi.o vauth/.libs/libcurl_la-krb5_sspi.o vauth/.libs/libcurl_la-ntlm.o vauth/.libs/libcurl_la-ntlm_sspi.o vauth/.libs/libcurl_la-oauth2.o vauth/.libs/libcurl_la-spnego_gssapi.o vauth/.libs/libcurl_la-spnego_sspi.o vauth/.libs/libcurl_la-vauth.o vtls/.libs/libcurl_la-bearssl.o vtls/.libs/libcurl_la-gskit.o vtls/.libs/libcurl_la-gtls.o vtls/.libs/libcurl_la-mbedtls.o vtls/.libs/libcurl_la-mbedtls_threadlock.o vtls/.libs/libcurl_la-mesalink.o vtls/.libs/libcurl_la-nss.o vtls/.libs/libcurl_la-openssl.o vtls/.libs/libcurl_la-schannel.o vtls/.libs/libcurl_la-schannel_verify.o vtls/.libs/libcurl_la-sectransp.o vtls/.libs/libcurl_la-vtls.o vtls/.libs/libcurl_la-wolfssl.o vquic/.libs/libcurl_la-ngtcp2.o vquic/.libs/libcurl_la-quiche.o vssh/.libs/libcurl_la-libssh.o vssh/.libs/libcurl_la-libssh2.o vssh/.libs/libcurl_la-wolfssh.o -L$${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib -lidn2 -lssl -lcrypto -lz  -g -pthread -Wl,--version-script=libcurl.vers   -pthread -Wl,-soname -Wl,libcurl.so.4 -o .libs/libcurl.so.4.6.0 -Wl,-rpath -Wl,${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib
  COMMAND ${CMAKE_COMMAND} -E copy .libs/libcurl.so.4.6.0 ${CMAKE_INSTALL_PREFIX}/curl_7.70.0/lib/libcurl.so.4.6.0
  DEPENDS ${CMAKE_INSTALL_PREFIX}/curl_7.70.0/lib/libcurl.so
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/curl_7.70.0/lib/libcurl.so
  COMMAND ./buildconf
  COMMAND ./configure --enable-debug --prefix=${CMAKE_INSTALL_PREFIX}/curl_7.70.0 --with-ssl=${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/ --enable-versioned-symbols
  COMMAND make -j 4
  COMMAND make install
  DEPENDS ${CURL_7.70.0_SRCS} 
  DEPENDS openssl_1.1.0h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)







## using add_custom_target

#[==[
add_custom_target(curl_7.70.0 ALL
  COMMAND ./buildconf
  COMMAND ./configure --enable-debug --prefix=${CMAKE_INSTALL_PREFIX}/curl_7.70.0 --with-ssl=${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/ --enable-versioned-symbols
  COMMAND make -j 4
  COMMAND make install
  DEPENDS ${CURL_7.70.0_SRCS} # for re-compilation when the source is modified
  DEPENDS ${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_custom_target(curl_7.70.0_STEP ALL
  COMMAND gcc -shared  -fPIC -DPIC  .libs/libcurl_la-altsvc.o .libs/libcurl_la-amigaos.o .libs/libcurl_la-asyn-ares.o .libs/libcurl_la-asyn-thread.o .libs/libcurl_la-base64.o .libs/libcurl_la-conncache.o .libs/libcurl_la-connect.o .libs/libcurl_la-content_encoding.o .libs/libcurl_la-cookie.o .libs/libcurl_la-curl_addrinfo.o .libs/libcurl_la-curl_ctype.o .libs/libcurl_la-curl_des.o .libs/libcurl_la-curl_endian.o .libs/libcurl_la-curl_fnmatch.o .libs/libcurl_la-curl_get_line.o .libs/libcurl_la-curl_gethostname.o .libs/libcurl_la-curl_gssapi.o .libs/libcurl_la-curl_memrchr.o .libs/libcurl_la-curl_multibyte.o .libs/libcurl_la-curl_ntlm_core.o .libs/libcurl_la-curl_ntlm_wb.o .libs/libcurl_la-curl_path.o .libs/libcurl_la-curl_range.o .libs/libcurl_la-curl_rtmp.o .libs/libcurl_la-curl_sasl.o .libs/libcurl_la-curl_sspi.o .libs/libcurl_la-curl_threads.o .libs/libcurl_la-dict.o .libs/libcurl_la-dotdot.o .libs/libcurl_la-easy.o .libs/libcurl_la-escape.o .libs/libcurl_la-file.o .libs/libcurl_la-fileinfo.o .libs/libcurl_la-formdata.o .libs/libcurl_la-ftp.o .libs/libcurl_la-url.o .libs/libcurl_la-ftplistparser.o .libs/libcurl_la-getenv.o .libs/libcurl_la-getinfo.o .libs/libcurl_la-gopher.o .libs/libcurl_la-hash.o .libs/libcurl_la-hmac.o .libs/libcurl_la-hostasyn.o .libs/libcurl_la-hostcheck.o .libs/libcurl_la-hostip.o .libs/libcurl_la-hostip4.o .libs/libcurl_la-hostip6.o .libs/libcurl_la-hostsyn.o .libs/libcurl_la-http.o .libs/libcurl_la-http2.o .libs/libcurl_la-http_chunks.o .libs/libcurl_la-http_digest.o .libs/libcurl_la-http_negotiate.o .libs/libcurl_la-http_ntlm.o .libs/libcurl_la-http_proxy.o .libs/libcurl_la-idn_win32.o .libs/libcurl_la-if2ip.o .libs/libcurl_la-imap.o .libs/libcurl_la-inet_ntop.o .libs/libcurl_la-inet_pton.o .libs/libcurl_la-krb5.o .libs/libcurl_la-ldap.o .libs/libcurl_la-llist.o .libs/libcurl_la-md4.o .libs/libcurl_la-md5.o .libs/libcurl_la-memdebug.o .libs/libcurl_la-mime.o .libs/libcurl_la-mprintf.o .libs/libcurl_la-mqtt.o .libs/libcurl_la-multi.o .libs/libcurl_la-netrc.o .libs/libcurl_la-non-ascii.o .libs/libcurl_la-nonblock.o .libs/libcurl_la-openldap.o .libs/libcurl_la-parsedate.o .libs/libcurl_la-pingpong.o .libs/libcurl_la-pop3.o .libs/libcurl_la-progress.o .libs/libcurl_la-psl.o .libs/libcurl_la-doh.o .libs/libcurl_la-rand.o .libs/libcurl_la-rename.o .libs/libcurl_la-rtsp.o .libs/libcurl_la-security.o .libs/libcurl_la-select.o .libs/libcurl_la-sendf.o .libs/libcurl_la-setopt.o .libs/libcurl_la-sha256.o .libs/libcurl_la-share.o .libs/libcurl_la-slist.o .libs/libcurl_la-smb.o .libs/libcurl_la-smtp.o .libs/libcurl_la-socketpair.o .libs/libcurl_la-socks.o .libs/libcurl_la-socks_gssapi.o .libs/libcurl_la-socks_sspi.o .libs/libcurl_la-speedcheck.o .libs/libcurl_la-splay.o .libs/libcurl_la-strcase.o .libs/libcurl_la-strdup.o .libs/libcurl_la-strerror.o .libs/libcurl_la-strtok.o .libs/libcurl_la-strtoofft.o .libs/libcurl_la-system_win32.o .libs/libcurl_la-telnet.o .libs/libcurl_la-tftp.o .libs/libcurl_la-timeval.o .libs/libcurl_la-transfer.o .libs/libcurl_la-urlapi.o .libs/libcurl_la-version.o .libs/libcurl_la-warnless.o .libs/libcurl_la-wildcard.o .libs/libcurl_la-x509asn1.o vauth/.libs/libcurl_la-cleartext.o vauth/.libs/libcurl_la-cram.o vauth/.libs/libcurl_la-digest.o vauth/.libs/libcurl_la-digest_sspi.o vauth/.libs/libcurl_la-krb5_gssapi.o vauth/.libs/libcurl_la-krb5_sspi.o vauth/.libs/libcurl_la-ntlm.o vauth/.libs/libcurl_la-ntlm_sspi.o vauth/.libs/libcurl_la-oauth2.o vauth/.libs/libcurl_la-spnego_gssapi.o vauth/.libs/libcurl_la-spnego_sspi.o vauth/.libs/libcurl_la-vauth.o vtls/.libs/libcurl_la-bearssl.o vtls/.libs/libcurl_la-gskit.o vtls/.libs/libcurl_la-gtls.o vtls/.libs/libcurl_la-mbedtls.o vtls/.libs/libcurl_la-mbedtls_threadlock.o vtls/.libs/libcurl_la-mesalink.o vtls/.libs/libcurl_la-nss.o vtls/.libs/libcurl_la-openssl.o vtls/.libs/libcurl_la-schannel.o vtls/.libs/libcurl_la-schannel_verify.o vtls/.libs/libcurl_la-sectransp.o vtls/.libs/libcurl_la-vtls.o vtls/.libs/libcurl_la-wolfssl.o vquic/.libs/libcurl_la-ngtcp2.o vquic/.libs/libcurl_la-quiche.o vssh/.libs/libcurl_la-libssh.o vssh/.libs/libcurl_la-libssh2.o vssh/.libs/libcurl_la-wolfssh.o -L$${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib -lidn2 -lssl -lcrypto -lz  -g -pthread -Wl,--version-script=libcurl.vers   -pthread -Wl,-soname -Wl,libcurl.so.4 -o .libs/libcurl.so.4.6.0 -Wl,-rpath -Wl,${CMAKE_INSTALL_PREFIX}/openssl_1.1.0h/lib
  COMMAND ${CMAKE_COMMAND} -E copy .libs/libcurl.so.4.6.0 ${CMAKE_INSTALL_PREFIX}/curl_7.70.0/lib/libcurl.so.4.6.0
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib
)
add_dependencies(curl_7.70.0 openssl_1.1.0h)
add_dependencies(curl_7.70.0_STEP curl_7.70.0)
#]==]

