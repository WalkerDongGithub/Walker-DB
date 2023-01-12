//
// Created by 董文阔 on 2022/9/23.
//

#ifndef WALKERDB_ROOTFILELAUNCHER_H
#define WALKERDB_ROOTFILELAUNCHER_H

#include "../Launcher.h"
#include "DDLEngine.h"
class RootFileLauncher : public Launcher {
    DDLEngine engine;
    Id root;
public:
    RootFileLauncher(FileSystem& fs): engine(fs) {}

    void Initialize() override {
        root = engine.MakeDir("root");
    }



};


#endif //WALKERDB_ROOTFILELAUNCHER_H
