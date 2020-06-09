#include <stdio.h>
#include <stdlib.h>

#include <gnutls/gnutls.h>
#include <assert.h>

#define CHECK(x) assert((x)>=0)

int main(void)
{
        printf("main started\n");
        /* for backwards compatibility with gnutls < 3.3.0 */
        CHECK(gnutls_global_init());
        gnutls_global_deinit();
        printf("main ended\n");
        return 0;

}
