/* SPDX-License-Identifier: LGPL-2.1-only */

#include "nl-default.h"

#include <linux-private/linux/netlink.h>
#include <linux-private/linux/if_link.h>

#include <netlink/route/link/macsec.h>
#include <netlink/netlink.h>
#include <netlink/route/link.h>

int main(int argc, char *argv[])
{
	struct rtnl_link *link;
	struct nl_cache *link_cache;
	struct nl_sock *sk;
	int err, master_index;

	sk = nl_socket_alloc();
	if ((err = nl_connect(sk, NETLINK_ROUTE)) < 0) {
		nl_perror(err, "Unable to connect socket");
		return err;
	}

	if ((err = rtnl_link_alloc_cache(sk, AF_UNSPEC, &link_cache)) < 0) {
		nl_perror(err, "Unable to allocate cache");
		return err;
	}

	if (!(master_index = rtnl_link_name2i(link_cache, "eth0"))) {
		fprintf(stderr, "Unable to lookup eth0");
		return -1;
	}


	link = rtnl_link_macsec_alloc();

	rtnl_link_set_link(link, master_index);

	rtnl_link_macsec_set_port(link, 10);
	rtnl_link_macsec_set_encrypt(link, 1);
	rtnl_link_macsec_set_replay_protect(link, 1);
	rtnl_link_macsec_set_window(link, 200);

	if ((err = rtnl_link_add(sk, link, NLM_F_CREATE)) < 0) {
		nl_perror(err, "Unable to add link");
		return err;
	}

	rtnl_link_put(link);
	nl_close(sk);

	return 0;
}
