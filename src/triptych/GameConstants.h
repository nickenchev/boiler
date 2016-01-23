//
//  GameConstants.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-07.
//
//

#ifndef triptych_GameConstants_h
#define triptych_GameConstants_h

#define USER_DEFAULT_KEY_HISCORE "hiscore"
#define USER_DEFAULT_KEY_TUTORIAL "main_tutorial"


#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#define INTERSTITIAL_ADUNIT_ID "ead06607f67a4de39491bcbb2a8d721c"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define INTERSTITIAL_ADUNIT_ID "bb44f37491ef4a70a58e84693fbf6180"
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#define BANNER_AD_UNIT_ID @"1b667ca0ccb54ba4a6352f1e2b7de9fc"
#define BANNER_IPAD_AD_UNIT_ID @"d2fd4164ca574c24b6a027308940b191"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define BANNER_AD_UNIT_ID @"47be0b8a941f4b9fa59264107a78e329"
#endif

#endif
