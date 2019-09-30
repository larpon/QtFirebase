# Examples
The following document assumes that you have a working [QtFirebase setup](SETUP.md)

More up-to-date and working QML examples can be found in the [QtFirebaseExample](https://github.com/Larpon/QtFirebaseExample/tree/3f19bf9/App/qml/pages) project.


## Analytics

### QML
```
import QtFirebase 1.0

Analytics {
    id: analytics

    // Analytics collection enabled
    enabled: true

    // App needs to be open at least 1s before logging a valid session
    minimumSessionDuration: 1000
    // App session times out after 5s (5 seconds = 5000 milliseconds)
    sessionTimeout: 5000

    // Set the user ID:
    // NOTE the user id can't be more than 36 chars long
    //userId: "A_VERY_VERY_VERY_VERY_VERY_VERY_LONG_USER_ID_WILL_BE_TRUNCATED"
    userId: "qtfirebase_test_user"
    // or call setUserId()

    // Unset the user ID:
    // userId: "" or call "unsetUserId()"

    // Set user properties:
    // Max 25 properties allowed by Google
    // See https://firebase.google.com/docs/analytics/cpp/properties
    userProperties: [
        { "sign_up_method" : "Google" },
        { "qtfirebase_power_user" : "yes" },
        { "qtfirebase_custom_property" : "test_value" }
    ]
    // or call setUserProperty()

    onReadyChanged: {
        // See: https://firebase.google.com/docs/analytics/cpp/events
        analytics.logEvent("qtfb_ready_event")
        analytics.logEvent("qtfb_ready_event","string_test","string")
        analytics.logEvent("qtfb_ready_event","int_test",getRandomInt(-100, 100))
        analytics.logEvent("qtfb_ready_event","double_test",getRandomArbitrary(-2.1, 2.7))

        analytics.logEvent("qtfb_ready_event_bundle",{
            'key_one': 'value',
            'key_two': 14,
            'key_three': 2.3
        })
    }
}
```

## Remote Config
Feature information available at https://firebase.google.com/docs/remote-config/

### QML
Remember to
* Add parameters in remote config section at google [firebase console](https://console.firebase.google.com)

```
import QtFirebase 1.0

RemoteConfig{
    id: remoteConfig

    //1. Initialize parameters you would like to fetch from server and their default values
    parameters: {
        "TestString" : "test",
        "TestDouble" : 2.56,
        "TestLong" : 1100,
        "TestBool" : true
    }

    //2. Set cache expiration time in milliseconds, see step 3 for details about cache
    cacheExpirationTime: 12*3600*1000 //12 hours in milliseconds (suggested as default in firebase)

    //3. When remote config properly initialized request data from server
    onReadyChanged: {
        console.log("RemoteConfig ready changed:"+ready);
        if(ready)
        {
            remoteConfig.fetch();
            //If the data in the cache was fetched no longer than cacheExpirationTime ago,
            //this method will return the cached data. If not, a fetch from the
            //Remote Config Server will be attempted.
            //If you need to get data urgent use fetchNow(), it is equal to fetch() call with cacheExpirationTime=0
            //Be careful with urgent requests, too often requests will result to server throthling
            //which means it will refuse connections for some time
        }
    }

    //4. If data was retrieved (both from server or cache) the handler will be called
    //you can access data by accessing the "parameters" member variable

    onParametersChanged:{
        console.log("RemoteConfig TestString:" + parameters['TestString']);
        console.log("RemoteConfig TestDouble:" + parameters['TestDouble']);
        console.log("RemoteConfig TestLong:" + parameters['TestLong']);
        console.log("RemoteConfig TestBool:" + parameters['TestBool']);
    }

    //5. Handle errors
    onError:{
        console.log("RemoteConfig error code:" + code + " message:" + message);
    }
}
```
