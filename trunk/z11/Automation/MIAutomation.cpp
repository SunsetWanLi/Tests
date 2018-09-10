//
//  MIAutomation.m
//  MIAutomation
//
//  Created by 王志鹏 on 16/8/11.
//  Copyright © 2016年 Rayme Tech Co.,LTD. All rights reserved.
//

#include "MIAutomation.h"
#include <vector>

using namespace std;
using namespace z11;

#pragma mark - MIAutomation -

Syncable MIAutomation::lock;
MIAutomation* MIAutomation::instance = NULL;
RaymeServer* MIAutomation::rayme_server = NULL;

bool MIAutomation::m_log_title = false;
bool MIAutomation::m_script_finish = true;

MIAutomation::MIAutomation()
{
    
}

MIAutomation::~MIAutomation()
{
    
}

MIAutomation* MIAutomation::createInstance()
{
    MIAutomation* ret = NULL;
    lock.lock();
    
    if (instance == NULL) {
        instance = new MIAutomation();
        assert(instance != NULL);
        instance->run();
    }
    ret = instance;
    
    if (rayme_server == NULL) {
        rayme_server = RaymeServer::initInstance(21100);
        assert(rayme_server != NULL);
        rayme_server->delegate = instance;
        rayme_server->run();
    }
    
    lock.unlock();
    return ret;
}

void MIAutomation::releaseInstance()
{
    lock.lock();
    
    if (instance != NULL) {
        instance->requestExitAndWait();
        delete_and_set_null(instance);
    }
    if (rayme_server != NULL)
        delete_and_set_null(rayme_server);
    
    lock.unlock();
}

ScriptStatus MIAutomation::initScriptWithFile(const string &script_file)
{
    ScriptStatus ret = SCRIPT_OK;
    lock.lock();
    
    try {
        std::ifstream file;
        file.open(script_file);
        if(file)
        {
            char str[4096];
            while (!file.eof()) {
                file.getline(str, sizeof(str));
                string bytes = str;
                RaymeRequest req;
                addScript(bytes, req);
            }
            file.close();
        } else {
            ret = SCRIPT_INVALID;			//  Cannot find file.
        }
    } catch (exception& e) {
        ret = SCRIPT_INVALID;				// Read file fail.
    }
    
    if (SCRIPT_OK == ret) {
        string file_name = script_file.substr(script_file.find_last_of("/")+1);
        logBegin();
        string title = "\r\n****************************************************************";
        title += "\r\n* OS : "+MGDevice::getDeviceOSType()+"\r\n"+
        "* Version : "+MGDevice::getDeviceOSVersion()+"\r\n"+
        "* Script : "+file_name;
        title += "\r\n****************************************************************";
        logString(title);
        logEnd();
        m_script_finish = false;
    }
    
    lock.unlock();
    resume();
    return ret;
}

ScriptStatus MIAutomation::addScript(const string &scr, const RaymeRequest& req)
{
    ScriptStatus ret = SCRIPT_OK;
    
    string script_cmd = scr;
    //Utility::TrimString(script_cmd);
    
    if (script_cmd.find("#") == 0) {
        ret = SCRIPT_NULL;
    }
    
    vector<string> strs = Utility::SplitString(script_cmd, ".");
    if (strs.front().compare("*") == 0 && strs.size() <= 2) {
        ret = SCRIPT_INVALID;
    }
    
    lock.lock();
    if (ret == SCRIPT_OK) {
        if (strs.size() == 0) {
            ret = SCRIPT_NULL;
        } else if (strs.size() == 1) {
            string str = strs.at(0);
            Utility::toLower(str);
            if (str.find("wait") == 0) {
                string::size_type bidx = str.find("(");
                string::size_type eidx = str.find(")");
                if (bidx == string::npos || eidx == string::npos || eidx <= bidx) {
                    ret = SCRIPT_INVALID;
                } else {
                    string time_str = str.substr(bidx+1, eidx-bidx-1);
                    MIAutoWait* wait = new MIAutoWait();
                    wait->req = req;
                    wait->wait_time = Utility::BuiltInTypeFromString<int_32>(time_str);
                    script.push_back(wait);
                }
            } else if (str.find("ui_tree_json") == 0) {
                MIAutoUITreeJson* tree = new MIAutoUITreeJson();
                tree->req = req;
                script.push_back(tree);
            } else if (str.find("ui_tree") == 0) {
                MIAutoUITree* tree = new MIAutoUITree();
                tree->req = req;
                script.push_back(tree);
            } else if (str.find("info") == 0) {
                strs.pop_back();
                MIAutoInfo* info = new MIAutoInfo();
                info->req = req;
                script.push_back(info);
            } else if (str.find("screen_shot") == 0) {
                strs.pop_back();
                MIAutoScreenShot* shot = new MIAutoScreenShot();
                shot->req = req;
                script.push_back(shot);
            } else if (str.find("lock") == 0) {
                strs.pop_back();
                MIAutoLock* lock = new MIAutoLock();
                lock->req = req;
                script.push_back(lock);
            } else if (str.find("unlock") == 0) {
                strs.pop_back();
                MIAutoUnlock* unlock = new MIAutoUnlock();
                unlock->req = req;
                script.push_back(unlock);
            } else if (str.find("h5_url") == 0) {
                strs.pop_back();
                MIAutoURL* url = new MIAutoURL();
                url->req = req;
                script.push_back(url);
            } else if (str.find("keyboard_shown") == 0) {
                strs.pop_back();
                MIAutoKeyboardShown* shown = new MIAutoKeyboardShown();
                shown->req = req;
                script.push_back(shown);
            } else if (str.find("tap") == 0) {
                string::size_type bidx = str.find("(");
                string::size_type eidx = str.find(")");
                if (bidx == string::npos || eidx == string::npos || eidx <= bidx) {
                    ret = SCRIPT_INVALID;
                } else {
                    string tap_point = str.substr(bidx+1, eidx-bidx-1);
                    vector<string> tap_strs = Utility::SplitString(tap_point, ",");
                    if (tap_strs.size() == 2) {
                        MIAutoTap* tap = new MIAutoTap();
                        tap->req = req;
                        int_32 x = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(0));
                        int_32 y = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(1));
                        tap->tap_point = MGIntPointMake(x, y);
                        script.push_back(tap);
                    } else if (tap_strs.size() == 3) {
                        MIAutoTap* tap = new MIAutoTap();
                        tap->req = req;
                        int_32 x = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(0));
                        int_32 y = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(1));
                        tap->tap_point = MGIntPointMake(x, y);
                        tap->interval_time = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(2));
                        script.push_back(tap);
                    } else
                        ret = SCRIPT_INVALID;
                }
            } else if (str.find("scroll") == 0) {
                string::size_type bidx = str.find("(");
                string::size_type eidx = str.find(")");
                if (bidx == string::npos || eidx == string::npos || eidx <= bidx) {
                    ret = SCRIPT_INVALID;
                } else {
                    string tap_point = str.substr(bidx+1, eidx-bidx-1);
                    vector<string> tap_strs = Utility::SplitString(tap_point, ",");
                    if (tap_strs.size() == 4) {
                        MIAutoScroll* scroll = new MIAutoScroll();
                        scroll->req = req;
                        int_32 x = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(0));
                        int_32 y = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(1));
                        scroll->init_point = MGIntPointMake(x, y);
                        x = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(2));
                        y = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(3));
                        scroll->scroll_x = x;
                        scroll->scroll_y = y;
                        script.push_back(scroll);
                    } else if (tap_strs.size() == 5) {
                        MIAutoScroll* scroll = new MIAutoScroll();
                        scroll->req = req;
                        int_32 x = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(0));
                        int_32 y = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(1));
                        scroll->init_point = MGIntPointMake(x, y);
                        x = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(2));
                        y = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(3));
                        scroll->scroll_x = x;
                        scroll->scroll_y = y;
                        scroll->interval_time = Utility::BuiltInTypeFromString<int_32>(tap_strs.at(4));
                        script.push_back(scroll);
                    } else
                        ret = SCRIPT_INVALID;
                }
            } else {
                ret = SCRIPT_INVALID;
            }
        } else {
            string str = strs.back();
            Utility::toLower(str);
            if (str.find("tap") == 0) {
                strs.pop_back();
                if (str.compare("tap") == 0) {
                    MIAutoTap* tap = new MIAutoTap();
                    tap->req = req;
                    for (auto it = strs.begin(); it != strs.end(); ++it) {
                        tap->addObjectChain(*it);
                    }
                    script.push_back(tap);
                } else {
                    string::size_type bidx = str.find("(");
                    string::size_type eidx = str.find(")");
                    if (bidx != string::npos && eidx != string::npos && bidx < eidx) {
                        string xy_str = str.substr(bidx+1, eidx-bidx-1);
                        vector<string> _xy = Utility::SplitString(xy_str, ",");
                        if (_xy.size() == 1) {
                            MIAutoTap* tap = new MIAutoTap();
                            tap->req = req;
                            for (auto it = strs.begin(); it != strs.end(); ++it) {
                                tap->addObjectChain(*it);
                            }
                            tap->interval_time = Utility::BuiltInTypeFromString<int_32>(_xy.at(0));
                            script.push_back(tap);
                        } else
                            ret = SCRIPT_INVALID;
                    } else
                        ret = SCRIPT_INVALID;
                }
            } else if (str.find("scroll") == 0) {
                strs.pop_back();
                string::size_type bidx = str.find("(");
                string::size_type eidx = str.find(")");
                if (bidx != string::npos && eidx != string::npos && bidx < eidx) {
                    string xy_str = str.substr(bidx+1, eidx-bidx-1);
                    vector<string> _xy = Utility::SplitString(xy_str, ",");
                    if (_xy.size() == 2) {
                        MIAutoScroll* scroll = new MIAutoScroll();
                        scroll->req = req;
                        for (auto it = strs.begin(); it != strs.end(); ++it) {
                            scroll->addObjectChain(*it);
                        }
                        scroll->scroll_x = Utility::BuiltInTypeFromString<int>(_xy.at(0));
                        scroll->scroll_y = Utility::BuiltInTypeFromString<int>(_xy.at(1));
                        script.push_back(scroll);
                    } else if (_xy.size() == 3) {
                        MIAutoScroll* scroll = new MIAutoScroll();
                        scroll->req = req;
                        for (auto it = strs.begin(); it != strs.end(); ++it) {
                            scroll->addObjectChain(*it);
                        }
                        scroll->scroll_x = Utility::BuiltInTypeFromString<int>(_xy.at(0));
                        scroll->scroll_y = Utility::BuiltInTypeFromString<int>(_xy.at(1));
                        scroll->interval_time = Utility::BuiltInTypeFromString<int_32>(_xy.at(2));
                        script.push_back(scroll);
                    } else
                        ret = SCRIPT_INVALID;
                } else {
                    ret = SCRIPT_INVALID;
                }
            } else if (str.find("text_in") == 0) {
                strs.pop_back();
                MIAutoTextIn* text_in = new MIAutoTextIn();
                text_in->req = req;
                for (auto it = strs.begin(); it != strs.end(); ++it) {
                    text_in->addObjectChain(*it);
                }
                string::size_type bidx = str.find("(");
                string::size_type eidx = str.find(")");
                if (bidx != string::npos && eidx != string::npos && bidx < eidx) {
                    text_in->text_in = str.substr(bidx+1, eidx-bidx-1);
                    script.push_back(text_in);
                } else {
                    ret = SCRIPT_INVALID;
                    delete_and_set_null(text_in);
                }
            } else if (str.find("text_out") == 0) {
                strs.pop_back();
                MIAutoTextOut* text_out = new MIAutoTextOut();
                text_out->req = req;
                for (auto it = strs.begin(); it != strs.end(); ++it) {
                    text_out->addObjectChain(*it);
                }
                script.push_back(text_out);
            } else if (str.find("exist") == 0) {
                strs.pop_back();
                MIAutoExist* exist = new MIAutoExist();
                exist->req = req;
                for (auto it = strs.begin(); it != strs.end(); ++it) {
                    exist->addObjectChain(*it);
                }
                script.push_back(exist);
            } else if (str.find("enabled") == 0) {
                strs.pop_back();
                MIAutoEnabled* enabled = new MIAutoEnabled();
                enabled->req = req;
                for (auto it = strs.begin(); it != strs.end(); ++it) {
                    enabled->addObjectChain(*it);
                }
                script.push_back(enabled);
            } else if (str.find("hide") == 0) {
                strs.pop_back();
                MIAutoHide* hide = new MIAutoHide();
                hide->req = req;
                for (auto it = strs.begin(); it != strs.end(); ++it) {
                    hide->addObjectChain(*it);
                }
                script.push_back(hide);
            } else if (str.find("show") == 0) {
                strs.pop_back();
                MIAutoShow* show = new MIAutoShow();
                show->req = req;
                for (auto it = strs.begin(); it != strs.end(); ++it) {
                    show->addObjectChain(*it);
                }
                script.push_back(show);
            } else if (str.find("ui_tree_json") == 0) {
                strs.pop_back();
                MIAutoUITreeJson* ui = new MIAutoUITreeJson();
                ui->req = req;
                for (auto it = strs.begin(); it != strs.end(); ++it) {
                    ui->addObjectChain(*it);
                }
                script.push_back(ui);
            } else if (str.find("ui_tree") == 0) {
                strs.pop_back();
                MIAutoUITree* ui = new MIAutoUITree();
                ui->req = req;
                for (auto it = strs.begin(); it != strs.end(); ++it) {
                    ui->addObjectChain(*it);
                }
                script.push_back(ui);
            } else if (str.find("ui_find") == 0) {
                strs.pop_back();
                MIAutoUIFind* ui = new MIAutoUIFind();
                ui->req = req;
                for (auto it = strs.begin(); it != strs.end(); ++it) {
                    ui->addObjectChain(*it);
                }
                script.push_back(ui);
            } else {
                ret = SCRIPT_INVALID;
            }
        }
    }
    if (ret != SCRIPT_OK) {
        MIAutoUnknown* unknown = new MIAutoUnknown();
        unknown->req = req;
        script.push_back(unknown);
    }
    
    lock.unlock();
    return ret;
}

ScriptStatus MIAutomation::executeScript()
{
    ScriptStatus ret = SCRIPT_OK;
    lock.lock();
    
    if (script.size() == 0) {
        if (m_script_finish == false) {
            logBegin();
            logString("\r\nScript finished.\r\n");
            logEnd();
            m_script_finish = false;
            m_script_finish = true;
        }
        pause();
    } else {
        auto it = script.front();
        if (!it->execute()) {
            ret = SCRIPT_FAIL;
            if (it->retry_num >= 2) {
                string data = it->actionScript()+"\tFAIL!!!";
                logString(data);
                script.pop_front();
                delete it;
                assert(rayme_server != NULL);
                rayme_server->handleAdapterResponse(it->req, data);
            } else
                it->retry_num++;
        } else {
            string data = it->actionScript()+"\tOK.";
            logString(data);
            MIAutoUITree* ui = dynamic_cast<MIAutoUITree*>(it);
            if (ui != NULL) {
                logBegin();
                logString("\r\n"+ui->ui_tree);
                logEnd();
                data.append("\r\n"+ui->ui_tree);
            }
            MIAutoUITreeJson* ui_json = dynamic_cast<MIAutoUITreeJson*>(it);
            if (ui_json != NULL) {
                logBegin();
                logString("\r\n"+ui_json->ui_tree);
                logEnd();
                data.append("\r\n"+ui_json->ui_tree);
            }
            MIAutoUIFind* find = dynamic_cast<MIAutoUIFind*>(it);
            if (find != NULL) {
                logBegin();
                logString("\r\n"+find->ui_pathes);
                logEnd();
                data.append("\r\n"+find->ui_pathes);
            }
            MIAutoTextOut* out = dynamic_cast<MIAutoTextOut*>(it);
            if (out != NULL) {
                logBegin();
                logString("\r\n"+out->text_out);
                logEnd();
                data.append("\r\n"+out->text_out);
            }
            MIAutoInfo* info = dynamic_cast<MIAutoInfo*>(it);
            if (info != NULL) {
                logBegin();
                logString("\r\n"+info->info);
                logEnd();
                data.append("\r\n"+info->info);
            }
            MIAutoScreenShot* shot = dynamic_cast<MIAutoScreenShot*>(it);
            if (shot != NULL) {
                data.append("\r\n");
                for (auto it = shot->img.begin(); it != shot->img.end(); ++it) {
                    data.push_back(*it);
                }
            }
            MIAutoURL* url = dynamic_cast<MIAutoURL*>(it);
            if (url != NULL) {
                logBegin();
                logString("\r\n"+url->url);
                logEnd();
                data.append("\r\n"+url->url);
            }
            MIAutoKeyboardShown* shown = dynamic_cast<MIAutoKeyboardShown*>(it);
            if (shown != NULL) {
                logBegin();
                logString("\r\n"+shown->shown);
                logEnd();
                data.append("\r\n"+shown->shown);
            }
            script.pop_front();
            delete it;
            assert(rayme_server != NULL);
            rayme_server->handleAdapterResponse(it->req, data);
        }
    }
    
    lock.unlock();
    return ret;
}

bool MIAutomation::threadLoop()
{
    bool ret = true;
    
    ScriptStatus status = executeScript();
    if (status != SCRIPT_OK) {
        MGSleep(500);
    } else {
        MGSleep(100);
    }
    
    return ret;
}

void MIAutomation::logBegin()
{
    m_log_title = true;
}

void MIAutomation::logString(const string& str)
{
    MGTime time = MGTime::Now();
    MGTime::Exploded ex;
    time.explode(ex, true);
    
    string path = MGBundle::mainBundle()->documentsPath() +"Log/";
    if (!Utility::isFileORDirExist(path)) {
        Utility::createDirOnPath(path);
    }
    string month = Utility::BuiltInTypeToString<int>(ex.month);
    if (month.size() == 1)
        month = "0"+month;
    string day = Utility::BuiltInTypeToString<int>(ex.day_of_month);
    if (day.size() == 1)
        day = "0"+day;
    string file_path = path + Utility::BuiltInTypeToString<int>(ex.year) + month + day + ".log";
    
    ofstream file;
    file.open(file_path.c_str(), ios::app | ios::out);
    if (file) {
        if (m_log_title)
        {
            file<<str<<endl;
        } else {
            file<<ex.hour<<":"<<ex.minute<<":"<<ex.second<<"> "<<str<<endl;
        }
        file.flush();
        file.close();
    }
}

void MIAutomation::logEnd()
{
    m_log_title = false;
}

void MIAutomation::handleRequest(const RaymeRequest &req)
{
    if (req.cmd.getType() != RaymeCOMMAND_REQUEST) {
        return;
    }
    
    string str = req.cmd.getContentStr();
    size_t pos = str.find("\r\n");
    while (pos != string::npos) {
        str = str.replace(pos, 2, "\n");
        pos = str.find("\r\n");
    }
    pos = str.find("\r");
    while (pos != string::npos) {
        str = str.replace(pos, 1, "\n");
        pos = str.find("\r");
    }
    
    vector<string> strs = Utility::SplitString(str, "\n");
    for (auto it = strs.begin(); it != strs.end(); ++it)
    {
        addScript(*it, req);
    }
    
    resume();
}
