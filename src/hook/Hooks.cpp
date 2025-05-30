#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#include "../util.h"

using namespace geode::prelude;

class $modify(PlayLayerHook, PlayLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point levelOpened;
    };
    
    void setupHasCompleted() {
        m_fields->levelOpened = std::chrono::steady_clock::now();
        PlayLayer::setupHasCompleted();
    }

    void onQuit() {
        log::debug("PlayLayer::onQuit");
        util::updateTime("in-game-playtime", m_fields->levelOpened);
        PlayLayer::onQuit();
    }
};

class $modify(PauseLayer) {
    void onEdit(CCObject* sender) {
        util::updateTime("in-game-playtime", static_cast<PlayLayerHook*>(PlayLayer::get())->m_fields->levelOpened);
        log::debug("PauseLayer::onEdit");
        PauseLayer::onEdit(sender);
    }
};

class $modify(EndLevelLayer) {
    void onEdit(CCObject* sender) {
        util::updateTime("in-game-playtime", static_cast<PlayLayerHook*>(PlayLayer::get())->m_fields->levelOpened);
        log::debug("EndLevelLayer::onEdit");
        EndLevelLayer::onEdit(sender);
    }
};

class $modify(LevelEditorLayerHook, LevelEditorLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point editorOpened;
    };

    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) return false;
        m_fields->editorOpened = std::chrono::steady_clock::now();
        return true;
    }
};

class $modify(EditorPauseLayerHook, EditorPauseLayer) {
    #ifdef GEODE_IS_MACOS
    void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
        if (p0->getTag() == 1 && p1) {
            util::updateTime("editor-playtime", static_cast<LevelEditorLayerHook*>(LevelEditorLayer::get())->m_fields->editorOpened);
            log::debug("FLAlert_Clicked");
        }
        EditorPauseLayer::FLAlert_Clicked(p0, p1);
    }

    void onSaveAndExit(CCObject* sender) {
        if (!m_saved) {
            util::updateTime("editor-playtime", static_cast<LevelEditorLayerHook*>(LevelEditorLayer::get())->m_fields->editorOpened);
            log::debug("onSaveAndExit");
        }
        EditorPauseLayer::onSaveAndExit(sender);
    }

    void onExitNoSave(CCObject* sender) {
        if (!m_saved && m_editorLayer->m_level->m_levelType != GJLevelType::Editor) {
            util::updateTime("editor-playtime", static_cast<LevelEditorLayerHook*>(LevelEditorLayer::get())->m_fields->editorOpened);
            log::debug("onExitNoSave");
        }
        EditorPauseLayer::onExitNoSave(sender);
    }
    #else
    void onExitEditor(CCObject* sender) {
        util::updateTime("editor-playtime", static_cast<LevelEditorLayerHook*>(LevelEditorLayer::get())->m_fields->editorOpened);
        log::debug("onExitEditor");
        EditorPauseLayer::onExitEditor(sender);
    }
    #endif

    void onSaveAndPlay(CCObject* sender) {
        util::updateTime("editor-playtime", static_cast<LevelEditorLayerHook*>(LevelEditorLayer::get())->m_fields->editorOpened);
        log::debug("onSaveAndPlay");
        EditorPauseLayer::onSaveAndPlay(sender);
    }
};