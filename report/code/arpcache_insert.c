void arpcache_insert(u32 ip4, u8 mac[ETH_ALEN])
{
    // 查找是否已有ip4对应的条目，若有，则替换掉
    int i = 0;
    int flag = 0;
    for (i = 0; i < MAX_ARP_SIZE; i += 1) {
        if(arpcache.entries[i].ip4 == ip4) {
            flag = 1;
            memcpy(arpcache.entries[i].mac, mac, ETH_ALEN);
            arpcache.entries[i].valid = 1;
            time(&arpcache.entries[i].added);
        }
    }

    if(flag == 0) {
        // 如果没有，创建一个，放到第一个的位置，其他项依次向后移动一位
        // 将最新的内容放到前面，先入先出
        struct arp_cache_entry *entry = (struct arp_cache_entry *)malloc(sizeof(struct arp_cache_entry));
        entry->ip4 = ip4;
        memcpy(entry->mac, mac, ETH_ALEN);
        time(&entry->added);
        entry->valid = 1;
        for (i = MAX_ARP_SIZE - 1; i >= 0; i -= 1) {
            arpcache.entries[i] = arpcache.entries[i - 1];
        }
        arpcache.entries[0] = *entry;
    }

    // 遍历缓存列表，如果有对应IP地址的待决包，将MAC地址填充好发送出去
    struct arp_req *req = NULL;
    struct cached_pkt *pkt = NULL;
    list_for_each_entry(req, &arpcache.req_list, list) {
        if(req->ip4 == ip4) {
            pkt = NULL;
            list_for_each_entry(pkt, &req->cached_packets, list) {
                struct ether_header * eh = (struct ether_header *)(pkt->packet);
                memcpy(eh->ether_dhost, mac, ETH_ALEN);
                iface_send_packet(req->iface, pkt->packet, pkt->len);
                list_delete_entry(&pkt->list);
            }
            list_delete_entry(&req->list);
        }
    }
}