#import "WifiStatus.h"
@implementation WifiStatus
- (id)init
{
    self = [super init];
    networks = [[NSMutableDictionary alloc] init];
    
    //libHandle = dlopen("/System/Library/SystemConfiguration/WiFiManager.bundle/WiFiManager", RTLD_LAZY);
    libHandle = dlopen("/System/Library/SystemConfiguration/IPConfiguration.bundle/IPConfiguration", RTLD_LAZY);
    char *error = NULL;
    if (libHandle == NULL && (error = dlerror()) != NULL)  {
        NSLog(@"%s",error);
        apple80211Scan = nil;
        apple80211Close = nil;
        airportHandle = nil;
        return self;
        //exit(1);
    }
    apple80211Open = dlsym(libHandle, "Apple80211Open");
    apple80211Bind = dlsym(libHandle, "Apple80211BindToInterface");
    apple80211Close = dlsym(libHandle, "Apple80211Close");
    apple80211Scan = dlsym(libHandle, "Apple80211Scan");
    apple80211Open(&airportHandle);
    apple80211Bind(airportHandle, @"en0");
    return self;
}

- (NSDictionary *)network:(NSString *) BSSID
{
    return [networks objectForKey:BSSID];
}

- (NSDictionary *)networks
{
    return networks;
}

- (void)scanNetworks
{
    NSLog(@"Scanning WiFi Channels…");
    NSDictionary *parameters = [[NSDictionary alloc] init];
    NSArray *scan_networks; //is a CFArrayRef of CFDictionaryRef(s) containing key/value data on each discovered network
    if (apple80211Scan == nil) {
        NSLog(@"Scanning WiFi Channels Failed.");
        return;
    }
    apple80211Scan(airportHandle, &scan_networks, parameters);
    NSLog(@"===–======\n%@",scan_networks);
    for (int i = 0; i < [scan_networks count]; i++) {
        [networks setObject:[scan_networks objectAtIndex: i] forKey:[[scan_networks objectAtIndex: i] objectForKey:@"BSSID"]];
    }
    NSLog(@"Scanning WiFi Channels Finished.");
}

- (int)numberOfNetworks
{
    return [networks count];
}

- ( NSString * ) description {
    NSMutableString *result = [[NSMutableString alloc] initWithString:@"Networks State: \n"];
    for (id key in networks){
        [result appendString:[NSString stringWithFormat:@"%@ (MAC: %@), RSSI: %@, Channel: %@ \n",
                              [[networks objectForKey: key] objectForKey:@"SSID_STR"], //Station Name
                              key, //Station BBSID (MAC Address)
                              [[networks objectForKey: key] objectForKey:@"RSSI"], //Signal Strength
                              [[networks objectForKey: key] objectForKey:@"CHANNEL"]  //Operating Channel
                              ]];
    }
    return [NSString stringWithString:result];
}

- (void) dealloc {
    if (apple80211Close != nil && airportHandle != nil) {
        apple80211Close(airportHandle);
    }
    [super dealloc];
}

@end