#ifndef CLICK_WIFIFRAGMENT_HH
#define CLICK_WIFIFRAGMENT_HH
#include <click/element.hh>
#include <clicknet/ether.h>
#include <click/etheraddress.hh>
#include <click/hashmap.hh>
CLICK_DECLS

/*
=c

WifiFragment

=s Wifi

Reassembles 802.11 fragments.

=d 

This element reassembles 802.11 fragments and pushes the reassembled
packet out when the last fragment is received. It does not affect
packets that are not fragmented, and passes them through its output
unmodified.  In accordance with the 802.11 spec, it supports a single
packet per SA.

=a WifiDecap
 */
class WifiFragment : public Element { public:
  
  WifiFragment();
  ~WifiFragment();

  const char *class_name() const	{ return "WifiFragment"; }
  const char *processing() const	{ return PUSH; }
  
  int configure(Vector<String> &, ErrorHandler *);
  bool can_live_reconfigure() const	{ return true; }

  void push(int, Packet *);


  void add_handlers();


  bool _debug;
  uint32_t _max_length;
  static String read_param(Element *e, void *thunk);
  static int write_param(const String &in_s, Element *e, void *vparam,
			 ErrorHandler *errh);
 private:

};

CLICK_ENDDECLS
#endif
