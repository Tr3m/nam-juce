#!/bin/bash
# Neural Amp Modeler (nam-juce) linux installer
# =====================================================
# =====================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# =====================================================

install()
{
    local vst_dir=$HOME/.vst3

    echo ""
    confirm "Would you like to add desktop integration for this install?"
    local desktop_integration=$?

    echo ""
    sudo -v
    echo""

    if [[ ! -d $vst_dir ]]; then
        mkdir -p $vst_dir
    fi

    echo Installing VST3 plug-in to $HOME/.vst3...
    cp -r "$SCRIPT_DIR/nam-juce/vst/Neural Amp Modeler.vst3" "$vst_dir/"


    echo Installing Standalone Application to /usr/bin...
    sudo cp $SCRIPT_DIR/nam-juce/bin/nam-juce /usr/bin
    
    if [[ $desktop_integration == 0 ]]; then
        do_desktop_ingegration /usr/bin/nam-juce
    fi

    echo ""
    echo "Finished!"
}

uninstall()
{
    echo ""
    sudo -v
    echo ""

    if command -v nam-juce >/dev/null 2>&1; then
        if sudo rm /usr/bin/nam-juce >/dev/null 2>&1; then
            echo Succesfully removed /usr/bin/nam-juce
        fi
    fi

    local vst_dir="$HOME/.vst3/Neural Amp Modeler.vst3"
    local module_info="$vst_dir/Contents/Resources/moduleinfo.json"

    if [[ -d "$vst_dir" ]] && [[ -f "$module_info" ]]; then
        if grep -q "Manos Labrakis" "$module_info"; then
            if rm -rf "$vst_dir" >/dev/null 2>&1; then
                echo Succesfully removed $vst_dir
            fi
        fi
    fi

    if [[ -f "$HOME/.local/share/applications/nam-juce.desktop" ]]; then
        if xdg-desktop-menu uninstall nam-juce.desktop >/dev/null 2>&1; then
            echo Succesfully removed Desktop Entry
        fi

        if xdg-icon-resource uninstall --size 128 nam-juce >/dev/null 2>&1; then
            echo Succesfully removed Desktop Icon 
        fi
    fi

    echo ""
    echo "Finished!"
    
}

do_desktop_ingegration()
{
    local tmp_dir="/tmp/nam-juce"

    if [[ -d $tmp_dir ]]; then
        rm -rf $tmp_dir
    fi

    mkdir $tmp_dir

    echo "Installing Desktop Entry..."

    make_desktop_entry\
		"Name=Neural Amp Modeler" \
		"Comment=A JUCE implementation of the Neural Amp Modeler Plugin." \
		"Categories=Audio" \
		"Exec=\"$1\"" \
		"Icon=nam-juce" \
		"StartupWMClass=nam-juce" \
		> "$tmp_dir/nam-juce.desktop"


	xdg-icon-resource install --size 128 "$SCRIPT_DIR/nam-juce/resources/icon.png" nam-juce 
    xdg-icon-resource forceupdate
	xdg-desktop-menu install "$tmp_dir/nam-juce.desktop"
    rm -rf $tmp_dir
}


make_desktop_entry() {
	echo "[Desktop Entry]"
	echo "Encoding=UTF-8"
	echo "Type=Application"
	while [ $# -gt 0 ]; do echo "$1"; shift; done
}

confirm()
{
    local answer

    while true; do
        read -r -p "${1:-$1} [Y/n] " answer
        answer=${answer:-y}

        case "$answer" in
            [Yy]) return 0 ;;
            [Nn]) return 1 ;;
            *) echo "Please answer y or n." ;;
        esac
    done
}

print_options()
{
    echo Available Options:
    echo "  1. View license agreement"
    echo ""
    echo "  2. Run nam-juce"
    echo ""
    echo "      Run the nam-juce standalone binary without installing it."
    echo ""
    echo "  3. Install nam-juce Standalone/VST3"
    echo ""
    echo "      The vst3 plug-in will be installed in $HOME/.vst3."
    echo "      The standalone application binary will be installed in /usr/bin."
    echo "      You will be prompted for optional desktop integration."
    echo ""
    echo "  4. Uninstall nam-juce"
    echo ""
    echo "  5. Add desktop integration for $SCRIPT_DIR/nam-juce/bin/nam-juce"
    echo ""
	echo "      If you've extracted nam-juce into the path you would like"
	echo "      it to live in ($SCRIPT_DIR/nam-juce), you can set"
	echo "      up desktop integration (menu items, file associations, "
	echo "      etc) via this action."
    echo ""
}

get_option()
{
    local prompt="$1"
    local min="$2"
    local max="$3"
    local answer

    while true; do
        read -r -p "$prompt [$min-$max] " answer

        if [[ "$answer" =~ ^[0-9]+$ ]] &&
           (( answer >= min && answer <= max )); then
            return "$answer"
        fi

        echo "Please choose an option between $min and $max."
    done
}

run_option()
{
    local choice="$1"

    case $choice in
        1)  # View license
            if command -v less >/dev/null 2>&1; then
                cat "$SCRIPT_DIR/nam-juce/license.rtf" | less
            else
                cat "$SCRIPT_DIR/nam-juce/license.rtf" | more
            fi
            clear
            print_options

            get_option "Command" 1 5

            selection=$?
            run_option $selection
            ;;
        2)
            echo Launching $SCRIPT_DIR/nam-juce/bin/nam-juce...
            $SCRIPT_DIR/nam-juce/bin/nam-juce
            ;;
        3)
            install
            ;;
        4)
            check_installed
            installed=$?

            if [[ $installed == 0 ]]; then
                uninstall
            else
                echo ""
                echo No nam-juce installation found!
            fi
            ;;

        5)
            echo""
            do_desktop_ingegration "$SCRIPT_DIR/nam-juce/bin/nam-juce"
            echo ""
            echo "Finished!"
            ;;
        *)
            ;;
    esac
}

check_installed()
{
    local binary_installed=0
    local vst_installed=0
    local desktop_integration=0

    if command -v nam-juce >/dev/null 2>&1; then
        binary_installed=1
    fi

    local vst_dir="$HOME/.vst3/Neural Amp Modeler.vst3"
    local module_info="$vst_dir/Contents/Resources/moduleinfo.json"

    if [[ -d $vst_dir ]] && [[ -f "$module_info" ]]; then
        if grep -q "Manos Labrakis" "$module_info"; then
            vst_installed=1
        fi
    fi

    if [[ -f "$HOME/.local/share/applications/nam-juce.desktop" ]]; then
        desktop_integration=1
    fi
    
    if [[ $binary_installed == 1 ]] || [[ $vst_installed == 1 ]] || [[ $desktop_integration == 1 ]];then
        return 0
    fi

    return 1

}

# =====================================================
# =====================================================

VERSION="PLUGIN_VERSION"

# =====================================================

echo "Neural Amp Modeler (nam-juce) v${VERSION} Installation script"
echo ---------------------------------------------------------

echo ""
print_options


get_option "Command" 1 5

selection=$?
run_option $selection
