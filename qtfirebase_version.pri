isEmpty(QTFIREBASE_GIT_VERSION_ROOT) {
  QTFIREBASE_GIT_VERSION_ROOT = $$PWD
}

# If there is no version tag this one will be used
isEmpty(QTFIREBASE_VERSION) {
  QTFIREBASE_VERSION = 0.0.0
}
isEmpty(QTFIREBASE_GIT_VERSION) {
  QTFIREBASE_GIT_VERSION = 0.0.0
}

QTFIREBASE_GIT_BRANCH = ""

# Need to discard STDERR so get path to NULL device
win32 {
    NULL_DEVICE = NUL # Windows doesn't have /dev/null but has NUL
} else {
    NULL_DEVICE = /dev/null
}

QTFIREBASE_GIT_BRANCH = $$system(git rev-parse --abbrev-ref HEAD)

# Need to call git with manually specified paths to repository
QTFIREBASE_BASE_GIT_COMMAND = git --git-dir $$QTFIREBASE_GIT_VERSION_ROOT/.git --work-tree $$QTFIREBASE_GIT_VERSION_ROOT

# Trying to get version from git tag / revision
QTFIREBASE_GIT_INITAL_VERSION = $$system($$QTFIREBASE_BASE_GIT_COMMAND describe --always --tags 2> $$NULL_DEVICE)
QTFIREBASE_GIT_VERSION = $$QTFIREBASE_GIT_INITAL_VERSION

# Check if we only have hash without version number
!contains(QTFIREBASE_GIT_VERSION, v*\d+\.\d+(\.\d+)*.* ) {
    # If there is nothing we simply use version defined manually
    isEmpty(QTFIREBASE_GIT_VERSION) {
        QTFIREBASE_GIT_VERSION = $$QTFIREBASE_VERSION
    } else { # otherwise construct proper git describe string
        QTFIREBASE_GIT_COMMIT_COUNT = $$system($$QTFIREBASE_BASE_GIT_COMMAND rev-list HEAD --count 2> $$NULL_DEVICE)
        isEmpty(QTFIREBASE_GIT_COMMIT_COUNT) {
            QTFIREBASE_GIT_COMMIT_COUNT = 0
        }
        QTFIREBASE_GIT_VERSION = $$VERSION-$$QTFIREBASE_GIT_COMMIT_COUNT-g$$QTFIREBASE_GIT_VERSION
    }
}

# Turns describe output like 0.1.5-42-g652c397 into "0.1.5.42.652c397"
QTFIREBASE_GIT_VERSION ~= s/-/"."
QTFIREBASE_GIT_VERSION ~= s/g/""

# Strip "v" prefix if any
QTFIREBASE_GIT_VERSION ~= s/^v/""

# Now we are ready to pass parsed version to Qt
# VERSION = $$GIT_VERSION
# win32 { # On windows version can only be numerical so remove commit hash
#    VERSION ~= s/\.\d+\.[a-f0-9]{6,}//
#}

# Adding C preprocessor #DEFINE so we can use it in C++ code
# also here we want full version on every system so using GIT_VERSION
DEFINES += \
    QTFIREBASE_VERSION=\\\"$$QTFIREBASE_VERSION\\\" \
    QTFIREBASE_GIT_VERSION=\\\"$$QTFIREBASE_GIT_VERSION\\\" \
    QTFIREBASE_GIT_BRANCH=\\\"$$QTFIREBASE_GIT_BRANCH\\\" \
\

# By default Qt only uses major and minor version for Info.plist on Mac.
# This will rewrite Info.plist with full version
#macx {
#    INFO_PLIST_PATH = $$shell_quote($${OUT_PWD}/$${TARGET}.app/Contents/Info.plist)
#    QMAKE_POST_LINK += /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $${VERSION}\" $${INFO_PLIST_PATH}
#}
