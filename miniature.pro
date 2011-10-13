include(config.pri)

TEMPLATE = subdirs
CONFIG += ordered qt

# Madde, another madly broken tool, cannot link against libraries from the same
# project correctly, horray! We only compile the lib outside of Madde.
!enable-madde-support {
    SUBDIRS += \
        game \
        tests \

}

SUBDIRS += \
    src \

TRANSLATIONS = src/frontend/miniature_miniaturets_en_US.ts \
               src/frontend/miniature_miniaturets_es.ts \
               src/frontend/miniature_miniaturets_de.ts
