//station.click

// This configuration contains a configuration for running 
// an 802.11b station (which can connect to an access point).
// It creates an interface using FromHost called "station"
// and uses open authentication to connect to an access point.
// This configuration assumes that you have a network interface named
// ath0 and that it is located on channel 1.

// Run it at user level with
// 'click station.click'

// Run it in the Linux kernel with
// 'click-install station.click'
// Messages are printed to the system log (run 'dmesg' to see them, or look
// in /var/log/messages), and to the file '/click/messages'.
// You will have to remove the elements ControlSocket and ChatterSocket to run
// in the kernel.

// To associate to an access point, load this config and then write the 
// following handlers:
// 
// winfo.channel <channel (integer)>
// winfo.bssid <access_point etheraddress>
// winfo.ssid <ssid of access point>
//
// And then trigger the assocation requests by writing the following handlers:
//
// station_auth.send_auth_req 1
// station_auth.send_assoc_req 1
//
// You should then see the following messages:
//
// station_auth :: OpenAuthRequester: auth 0 seq 2 status 0
// station_assoc :: AssociationRequester: response 00:12:17:1c:f1:b1 +24 [ ESS ] status 0 associd 49156 ( { 2 4 11 22 } 36 48 72 108 )
//
// status of 0 means you associated/authenticated successfully.
//


control :: ControlSocket("TCP", 7777);
chatter :: ChatterSocket("TCP", 7778);

AddressInfo(station_address 192.168.1.1/24 ath0);
winfo :: WirelessInfo(SSID "", BSSID 00:00:00:00:00:00, CHANNEL 1);

rates :: AvailableRates(DEFAULT 2 4 11 22);

q :: Queue(10)
-> SetTXRate(2)
-> SetTXPower(63)
-> extra_encap :: ExtraEncap()
-> ToDevice (ath0);


FromDevice(ath0)
-> prism2_decap :: Prism2Decap()
-> extra_decap :: ExtraDecap()
-> FilterPhyErr()
-> tx_filter :: FilterTX()
-> dupe :: WifiDupeFilter()
-> wifi_cl :: Classifier(0/00%0c, //mgt
			 0/08%0c, //data
			 );

// management
wifi_cl [0] -> management_cl :: Classifier(0/00%f0, //assoc req
					   0/10%f0, //assoc resp
					   0/80%f0, //beacon
					   0/a0%f0, //disassoc
					   0/50%f0, //probe resp
					   0/b0%f0, //auth
					   );
			     


station_probe :: ProbeRequester(ETH station_address,
				WIRELESS_INFO winfo,
				RT rates) 
-> PrintWifi(probe-req) -> Print(probe-req) -> q;

station_auth :: OpenAuthRequester(ETH station_address, WIRELESS_INFO winfo) 
-> PrintWifi(auth-req) 
-> q;

station_assoc ::  AssociationRequester(ETH station_address,
				       WIRELESS_INFO winfo,
				       RT rates) 
-> Print("sent assoc_req", 60) 
-> PrintWifi(assoc-req) 
-> q;

management_cl [0] -> Print ("assoc_req") -> Discard;
management_cl [1] -> Print ("assoc_resp", 60) -> station_assoc;
management_cl [2] -> beacon_t :: Tee(2) 
-> bs :: BeaconScanner(RT rates, WIRELESS_INFO winfo) ->  Discard;
beacon_t [1] -> tracker :: BeaconTracker(WIRELESS_INFO winfo, TRACK 10) -> Discard;

management_cl [3] -> Print ("dissoc") -> station_assoc;
management_cl [4] -> PrintWifi(probe_resp) -> bs;
management_cl [5] -> PrintWifi(auth) -> station_auth;

wifi_cl [1] 
//-> PrintWifi(data)
-> wifi_decap :: WifiDecap() 
-> ToHost(station);


FromHost(station, station_address, ETHER station_address)
-> station_encap :: WifiEncap(0x01, WIRELESS_INFO winfo)
-> q;


