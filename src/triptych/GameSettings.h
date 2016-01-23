//
//  GameSettings.h
//  triptych
//
//  Created by Nick Enchev on 2014-10-06.
//
//

#ifndef triptych_GameSettings_h
#define triptych_GameSettings_h

class GameSettings
{
    GameSettings()
    {
        shouldRestart = false;
    }
    
public:
    static GameSettings &sharedInstance()
    {
        static GameSettings instance;
        return instance;
    }
    
    GameSettings(GameSettings const &);
    void operator=(GameSettings const &);
    
    bool shouldRestart;
};


#endif
