#ifndef IP6NDADVERTISER_HH
#define IP6NDADVERTISER_HH

/*
 * =c
 * IP6NDAdvertiser(IP61 MASK1 ETH1, IP62 MASK2 ETH2, ...)
 * =s IPv6
 * 
 * =d
 * Input should be Neighbor Solitation Message, which includes
 * the ethernet header, ip6 header and message itself. The Neighbor  
 * Solitation Message query about the link layer address of an IPv6 
 * target address. If the IP6NDAdvertiser knows the answer, it 
 * forwards an Neighbor Advertisement Message.
 * if we know the answer.
 * Could be used for proxy ARP as well as producing
 * replies for a host's own address.
 *
 * =e
 * Produce Neighborhood Advertisement for the local machine (3ffe:1ce1:2::5)
 * as well as proxy ARP for all machines on net 3ffe:1ce1:2::/64)
 * directing their packets to the local machine:
 *
 *   c :: Classifier(12/86dd 54/87, ...);
 *   ndadv :: IP6NDAdvertiser(3ffe:1ce1:2::5/128 00:00:C0:AE:67:EF,
 *               3ffe:1ce1:2::/80 00:00:C0:AE:67:EF)
 *   c[0] -> ndadv;
 *   ndadv -> ToDevice(eth0);
 *
 * =a
 * IP6NDSolicitor
 */

#include <click/element.hh>
#include <click/etheraddress.hh>
#include <click/ip6address.hh>
#include <click/vector.hh>

class IP6NDAdvertiser : public Element {
public:
  IP6NDAdvertiser();
  ~IP6NDAdvertiser();
  
  const char *class_name() const		{ return "IP6NDAdvertiser"; }
  const char *processing() const		{ return AGNOSTIC; }
  IP6NDAdvertiser *clone() const;
  int configure(const Vector<String> &, ErrorHandler *);

  Packet *simple_action(Packet *);

  //response to multicast and unicast Neighbor Solitation message 
  // which is querying the ethernet address of the targest IP6 address
  Packet *make_response(unsigned char dha[6], unsigned char sha[6],
                        unsigned char dpa[16], unsigned char spa[16],
			unsigned char tpa[16], unsigned char tha[6]);


  //response to unicast Neighbor Solitation message only
  //which is veryfying the ethernet address of the targest IP6 address
  Packet *make_response2(unsigned char dha[6], unsigned char sha[6],
                        unsigned char dpa[16], unsigned char spa[16],
			unsigned char tpa[16]);

  bool lookup(IP6Address, EtherAddress &);

private:

  struct Entry {
    IP6Address _dst;
    IP6Address _mask;
    EtherAddress _ena;
  };
  Vector<Entry> _v;
  
  void add_map(IP6Address dst, IP6Address mask, EtherAddress);

};

#endif
