#!/bin/bash
#.localized workaround for standalone app

EXPECTED_BUNDLE_IDS=(
    "com.Manos Labrakis.NEURAL_AMP_MODELER"
    "com.ManosLabrakis.NEURAL_AMP_MODELER"
    "com.ManosLabrakis.NAM_JUCE"
)

EXPECTED_PACKAGE_IDS=(
    "NeuralAmpModeler"
    "nam-juce"
)

move_if_matching_bundle() {
    local TARGET="/Applications/Neural Amp Modeler.localized"
    shift
    local ACTUAL_TARGET="$TARGET/Neural Amp Modeler.app"
    shift
    local EXPECTED_BUNDLE_IDS=("$@")

    if [ ! -e "$TARGET" ]; then
        return 1
    fi

    local INFO_PLIST="$ACTUAL_TARGET/Contents/Info.plist"

    if [ ! -f "$INFO_PLIST" ]; then
        echo "ERROR: $ACTUAL_TARGET exists but has no Info.plist."
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
            echo "Moving existing $ACTUAL_TARGET (bundle ID: $BUNDLE_ID)"
            mv "$ACTUAL_TARGET" /Applications/
            rm -rf "$TARGET"

            return 0
        fi
    done

    echo "ERROR: $ACTUAL_TARGET exists with unexpected bundle ID '$BUNDLE_ID'."
    echo "Refusing to remove it."
    return 1
}

if move_if_matching_bundle "${EXPECTED_BUNDLE_IDS[@]}"; then
    echo "Moved Neural Amp Modeler.app to /Applications!"
fi

exit 0
