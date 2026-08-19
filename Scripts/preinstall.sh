#!/bin/bash

APP="/Applications/Neural Amp Modeler.app"
VST="/Library/Audio/Plug-Ins/VST/Neural Amp Modeler.vst3"
AU="/Library/Audio/Plug-Ins/Components/Neural Amp Modeler.component"

EXPECTED_BUNDLE_IDS=(
    "com.Manos Labrakis.NEURAL_AMP_MODELER"
    "com.ManosLabrakis.NEURAL_AMP_MODELER"
    "com.ManosLabrakis.NAM_JUCE"
)

EXPECTED_PACKAGE_IDS=(
    "NeuralAmpModeler"
    "nam-juce"
)

forget_packages() {
    local TARGET=$1
    shift
    local EXPECTED_PACKAGE_IDS=("$@")

    for PACKAGE_ID in "${EXPECTED_PACKAGE_IDS[@]}"; do
        local FULL_ID="com.ManosLabrakis.$TARGET.pkg.$PACKAGE_ID"
        if pkgutil --pkg-info "$FULL_ID" >/dev/null 2>&1; then
            echo "Forgetting package: $FULL_ID"

            if ! pkgutil --forget "$FULL_ID"; then
                echo "ERROR: Failed to forget package: $FULL_ID"
                return 1
            fi
        fi
    done

    return 0
}

remove_if_matching_bundle() {
    local TARGET="$1"
    shift
    local EXPECTED_BUNDLE_IDS=("$@")

    if [ ! -e "$TARGET" ]; then
        return 1
    fi

    local INFO_PLIST="$TARGET/Contents/Info.plist"

    if [ ! -f "$INFO_PLIST" ]; then
        echo "ERROR: $TARGET exists but has no Info.plist."
        return 1
    fi

    local BUNDLE_ID
    BUNDLE_ID=$(/usr/libexec/PlistBuddy \
        -c "Print :CFBundleIdentifier" \
        "$INFO_PLIST" 2>/dev/null) || {
            echo "ERROR: Could not read bundle ID from $INFO_PLIST"
            return 1
        }

    for EXPECTED_ID in "${EXPECTED_BUNDLE_IDS[@]}"; do
        if [ "$BUNDLE_ID" = "$EXPECTED_ID" ]; then
            echo "Removing existing $TARGET (bundle ID: $BUNDLE_ID)"
            rm -rf "$TARGET"
            return 0
        fi
    done

    echo "ERROR: $TARGET exists with unexpected bundle ID '$BUNDLE_ID'."
    echo "Refusing to remove it."
    return 1
}

