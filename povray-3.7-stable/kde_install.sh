#!/bin/sh
# ==================================================================
# POV-Ray 3.7 - Unix source version - KDE install script
# ==================================================================
# written July 2003 - March 2004 by Christoph Hormann
# Based on parts of the Linux binary version install script
# This file is part of POV-Ray and subject to the POV-Ray licence
# see POVLEGAL.DOC for details.
# ==================================================================


# @@KDE_BEGIN@@

POVRAY=POV-Ray
POV_DIR=povray
VERSION_FULL=3.7.0
VERSION=3.7
VER_DIR=$POV_DIR-$VERSION
TARGET_PLATFORM=x86
TARGET_PLATFORM_NAME="x86 Linux"
#TARGET_PLATFORM=x86_64
#TARGET_PLATFORM_NAME="x86_64 (AMD64) Linux"
DEFAULT_DIR=/usr/local
SYSCONFDIR=$DEFAULT_DIR/etc
POVLINUX_DIR=`dirname $0`
CDIR=`pwd`

if [ -z "$TMPDIR" ] ; then
	TMPDIR="/tmp"
fi

if [ ! -w  "$TMPDIR" ] ; then
	TMPDIR=$CDIR
fi

TMP_LOG="$TMPDIR/${VER_DIR}_`date +%s`.log"
test -w "$TMP_LOG" && rm -f "$TMP_LOG"

if [ ! -z "$DISPLAY" ] ; then
	KDIALOG=`which kdialog`
fi

TEXT_MODE=
test "$1" = "text" && TEXT_MODE=on
test "$2" = "text" && TEXT_MODE=on
test "$3" = "text" && TEXT_MODE=on
test "$4" = "text" && TEXT_MODE=on
test "$5" = "text" && TEXT_MODE=on
test "$6" = "text" && TEXT_MODE=on
test "$7" = "text" && TEXT_MODE=on
test "$8" = "text" && TEXT_MODE=on
test "$9" = "text" && TEXT_MODE=on

if [ ! -z "$TEXT_MODE" ] ; then
	KDIALOG=
fi

QUIET_MODE=
test "$1" = "quiet" && QUIET_MODE=on
test "$2" = "quiet" && QUIET_MODE=on
test "$3" = "quiet" && QUIET_MODE=on
test "$4" = "quiet" && QUIET_MODE=on
test "$5" = "quiet" && QUIET_MODE=on
test "$6" = "quiet" && QUIET_MODE=on
test "$7" = "quiet" && QUIET_MODE=on
test "$8" = "quiet" && QUIET_MODE=on
test "$9" = "quiet" && QUIET_MODE=on


# [NC] POSIX-compliant read from terminal
read_input()
{
	# print a message before reading input
	if test x"`printf %s check 2> /dev/null`" = x"check"; then
		printf %s "$2"
	else	# will print a newline
		echo "$2"
	fi
	read $1
}


# ==================================================================
#    messages - alternatively console or kdialog
# ==================================================================

info_message()
{
	echo "$1" 2>&1 | tee -a "$TMP_LOG"
	if [ ! -z "$KDIALOG" ] ; then
		kdialog --caption "$POVRAY $VERSION_FULL installation" --msgbox "$1"
	fi
}

error_message()
{
	echo "------------------------------------------------------" 2>&1 | tee -a "$TMP_LOG"
	echo "Error: $1" 2>&1 | tee -a "$TMP_LOG"
	echo "------------------------------------------------------" 2>&1 | tee -a "$TMP_LOG"
	if [ ! -z "$KDIALOG" ] ; then
		kdialog --caption "$POVRAY $VERSION_FULL installation" --error "$1"
	fi
}

# ==================================================================
#    abort install and display log if requested
# ==================================================================

abort_install()
{
	echo "Installation aborted!" 2>&1 | tee -a "$TMP_LOG"

	if [ ! -z "$KDIALOG" ] ; then
		kdialog --caption "$POVRAY $VERSION_FULL installation" --yesno "The installation of $POVRAY $VERSION_FULL has been aborted due to an error or user request.  Do you want to view the installation log?"
		if [ $? -ne 0 ] ; then
			exit
		else
			kdialog --caption "$POVRAY $VERSION_FULL installation - Installation log" --textbox "$TMP_LOG" 800 600
			exit
		fi
	else
		echo "" 2>&1 | tee -a "$TMP_LOG"
		exit
	fi

}

# ==================================================================
#    finish and display log if requested
# ==================================================================

finish_install()
{
	echo "Installation finished successfully!" 2>&1 | tee -a "$TMP_LOG"

	if [ ! -z "$KDIALOG" ] ; then
		kdialog --caption "$POVRAY $VERSION_FULL installation" --yesno "The installation of $POVRAY $VERSION_FULL has been finished successfully.  Do you want to view the installation log?"
		if [ $? -ne 0 ] ; then
			exit
		else
			kdialog --caption "$POVRAY $VERSION_FULL installation" --textbox "$TMP_LOG" 800 600
			exit
		fi
	else
		echo "" 2>&1 | tee -a "$TMP_LOG"
		exit
	fi

}

# ==================================================================
#    check system and package content
# ==================================================================

prepare_bin_install()
{

	INCOMPLETE=
	test ! -r "$POVLINUX_DIR/povray" && INCOMPLETE=true
	test ! -r "$POVLINUX_DIR/povray.ini" && INCOMPLETE=true
	test ! -r "$POVLINUX_DIR/povray.conf" && INCOMPLETE=true
	test ! -d "$POVLINUX_DIR/scenes" && INCOMPLETE=true
	test ! -d "$POVLINUX_DIR/include" && INCOMPLETE=true
	test ! -d "$POVLINUX_DIR/doc" && INCOMPLETE=true
	test ! -d "$POVLINUX_DIR/ini" && INCOMPLETE=true
	test ! -d "$POVLINUX_DIR/scripts" && INCOMPLETE=true

	if [ ! -z "$INCOMPLETE" ] ; then
		error_message "files required for installation not found -
make sure you correctly downloaded and unpacked 
the installation package"
		abort_install
	fi

	if [ -z "$QUIET_MODE" ] ; then
		if [ ! -z "$KDIALOG" ] ; then
			kdialog --caption "$POVRAY $VERSION_FULL installation" --yesno "welcome to the $POVRAY $VERSION_FULL installation.\nThe install script has detected a KDE installation on this system and will use kdialog.  If you prefer a text mode install use 'install text'.  Start installation now?"
			if [ $? -ne 0 ] ; then
				abort_install
			fi
		fi
	fi

	echo "installation preparation successful" >> "$TMP_LOG"

}

# ==================================================================
#    select sys/user install and obtain install dir
# ==================================================================

select_target_dir()
{
	POVRAY_BINARY=`which povray`

	if [ ! -w "$DEFAULT_DIR" ] ; then
		if [ ! -z "$KDIALOG" ] ; then
			if [ ! -z "$POVRAY_BINARY" ] ; then
				ADD_MESSAGE="
Note there is currently a $POVRAY binary installed on the system ($POVRAY_BINARY).  You need to set the PATH environment variable or rename this or the new binary to make sure the correct version is called when running 'povray'."
			else
				ADD_MESSAGE=
			fi
			kdialog --caption "$POVRAY $VERSION_FULL installation" --yesno "You need to have root privileges to install $POVRAY $VERSION_FULL in the default location ($DEFAULT_DIR).\nYou can also install $POVRAY on user level at a custom location but this requires additional manual setup steps.  Do you want to login as root now to perform a system install?  If you select 'no' you will be asked for a custom location for a user install instead.  $ADD_MESSAGE"

			if [ $? -eq 0 ] ; then
				OPTS=
				test -z "$TEXT_MODE" || OPTS="text"
				test -z "$SKIP_ARCH" || OPTS="-no-arch-check $OPTS"
				kdesu -t -c $0 $OPTS quiet
				exit
			else
				BASEDIR=`kdialog --inputbox "Please specify the base directory you want to install in.
This directory has to be writable from this user account.
You will probably want to speciafy a location in your home
directory (like $HOME/usr):" "$HOME/usr" --title "$POVRAY $VERSION_FULL installation"` 2> /dev/null

				if [ $? -ne 0 ] ; then
					abort_install
				fi

				BASEDIR=`echo "$BASEDIR" | sed "s?~?$HOME?g"`

				if [ ! -d "$BASEDIR" ] ; then
					mkdir -p "$BASEDIR"
				fi

				if [ ! -w "$BASEDIR" ] ; then
					error_message "The chosen install directory is not writable, 
either login as root or specify a different location."
					abort_install
				fi
			fi
		else
			if [ -z "$POVRAY_BINARY" ]  ; then
				ADD_MESSAGE=
			else
				if [ "$POVRAY_BINARY" = "$DEFAULT_DIR/bin/povray" ]  ; then
					ADD_MESSAGE=
				else
					ADD_MESSAGE="
Note there is currently a $POVRAY binary installed on 
the system ($POVRAY_BINARY).  You need to set the PATH
environment variable or rename this or the new binary
to make sure the correct version is called when 
running 'povray'.
"
				fi
			fi
			echo "You need to have root privileges to install $POVRAY $VERSION_FULL"
			echo "in the default location ($DEFAULT_DIR)."
			echo ""
			echo "You can also install $POVRAY on user level at a custom location"
			echo "but this requires additional manual setup steps."
			echo "$ADD_MESSAGE"
			echo "  Type 'R'  to login as root and install in $DEFAULT_DIR"
			echo "            (recommended method)."
			echo "  Type 'U'  to make a user level installation at a custom location."
			echo "  Type anything else to abort."
			echo ""
			read_input INPUT "Your choice (R/U, default: abort): "

			echo ""

			case $INPUT in
				"r" | "R")                 # login as root
					echo ""
					echo "Enter root password to install in $DEFAULT_DIR:"
					OPTS=
					test -z "$TEXT_MODE" || OPTS="text"
					test -z "$SKIP_ARCH" || OPTS="-no-arch-check $OPTS"
					su -c $0 $OPTS quiet
					exit
					;;
				"u" | "U")                 # ask for location and continue
					echo ""
					echo "Please specify the base directory you want to install in."
					echo "This directory has to be writable from this user account."
					echo "You will probably want to speciafy a location in your home"
					echo "directory (like $HOME/usr):"

					read_input BASEDIR "directory name: "

					echo ""

					BASEDIR=`echo "$BASEDIR" | sed "s?~?$HOME?g"`

					if [ ! -d "$BASEDIR" ] ; then
						mkdir -p "$BASEDIR"
					fi

					if [ ! -w "$BASEDIR" ] ; then

						echo ""
						echo "This directory is not writable, either login as root"
						echo "or specify a different location."
						echo ""
						echo "Installation aborted!"
						exit
					fi
					;;
				*)                         # abort
					abort_install
					;;
			esac
		fi
	else
		echo "installing $POVRAY in default location ($DEFAULT_DIR)" 2>&1 | tee -a "$TMP_LOG"
		BASEDIR=$DEFAULT_DIR
	fi

	echo "installation directory selected: $BASE_DIR" >> "$TMP_LOG"
}

# ==================================================================
#    Add read+write path to user povray.conf file
# ==================================================================

add_readwrite_conf()
{
	DIR_NAME=$1
	CONF_FILE="$HOME/.$POV_DIR/$VERSION/povray.conf"

	echo "  checking conf file $CONF_FILE" 2>&1 | tee -a "$TMP_LOG"

	if [ ! -r "$CONF_FILE" ] ; then
		cp -f "$SYSCONFDIR/$POV_DIR/$VERSION/povray.conf" "$CONF_FILE" 2>&1 | tee -a "$TMP_LOG"
	fi

	if [ -w "$CONF_FILE" ] ; then

		if grep -E -i "$DIR_NAME" "$CONF_FILE" > /dev/null ; then
			echo "    - file does not need to be modified" 2>&1 | tee -a "$TMP_LOG"
		else
			echo "    - adding new read+write path" 2>&1 | tee -a "$TMP_LOG"

			cp -f "$CONF_FILE" "$CONF_FILE.bak" 2>&1 | tee -a "$TMP_LOG"

			grep -B 1000 -E -i "^\[Permitted Paths\]" "$CONF_FILE.bak" > "$CONF_FILE" 2> /dev/null

			echo ";--- Lines added by $POVRAY $VERSION_FULL install script ---" >> "$CONF_FILE"
			echo "read+write* =  \"$DIR_NAME\"" >> "$CONF_FILE"
			echo ";---------------------------------------------------" >> "$CONF_FILE"

			grep -A 1000 -E -i "^\[Permitted Paths\]" "$CONF_FILE.bak" | sed "/^\[Permitted Paths\]/d" >> "$CONF_FILE" 2> /dev/null

			rm -f "$CONF_FILE.bak" 2>&1 | tee -a "$TMP_LOG"

		fi

	else
		echo "Error: could not modify povray.conf" 2>&1 | tee -a "$TMP_LOG"
	fi
}

# ==================================================================
#    Add read path to user povray.conf file
# ==================================================================

add_read_conf()
{
	DIR_NAME=$1
	CONF_FILE="$HOME/.$POV_DIR/$VERSION/povray.conf"

	echo "  checking conf file $CONF_FILE" 2>&1 | tee -a "$TMP_LOG"

	if [ ! -r "$CONF_FILE" ] ; then
		cp -f "$SYSCONFDIR/$POV_DIR/$VERSION/povray.conf" "$CONF_FILE" 2>&1 | tee -a "$TMP_LOG"
	fi

	if [ -w "$CONF_FILE" ] ; then

		if grep -E -i "$DIR_NAME" "$CONF_FILE" > /dev/null ; then
			echo "    - file does not need to be modified" 2>&1 | tee -a "$TMP_LOG"
		else
			echo "    - adding new read path" 2>&1 | tee -a "$TMP_LOG"

			cp -f "$CONF_FILE" "$CONF_FILE.bak" 2>&1 | tee -a "$TMP_LOG"

			grep -B 1000 -E -i "^\[Permitted Paths\]" "$CONF_FILE.bak" > "$CONF_FILE" 2> /dev/null

			echo ";--- Lines added by $POVRAY $VERSION_FULL install script ---" >> "$CONF_FILE"
			echo "read* =  \"$DIR_NAME\"" >> "$CONF_FILE"
			echo ";---------------------------------------------------" >> "$CONF_FILE"

			grep -A 1000 -E -i "^\[Permitted Paths\]" "$CONF_FILE.bak" | sed "/^\[Permitted Paths\]/d" >> "$CONF_FILE" 2> /dev/null

			rm -f "$CONF_FILE.bak" 2>&1 | tee -a "$TMP_LOG"

		fi

	else
		echo "Error: could not modify povray.conf" 2>&1 | tee -a "$TMP_LOG"
	fi
}

# ==================================================================
#    Determine installation dir from Library_path settings in ini
# ==================================================================

install_dir()
{
  if [ -z "$POVINI" ] ; then
    test -r "$SYSCONFDIR/povray.ini" && POVINI="$SYSCONFDIR/povray.ini"
    test -r "$HOME/.povrayrc" && POVINI="$HOME/.povrayrc"
    test -r "$SYSCONFDIR/$POV_DIR/$VERSION/povray.ini" && POVINI="$SYSCONFDIR/$POV_DIR/$VERSION/povray.ini"
    test -r "$HOME/.$POV_DIR/$VERSION/povray.ini" && POVINI="$HOME/.$POV_DIR/$VERSION/povray.ini"
  fi

  if [ ! -z "$POVINI" ] ; then
    # this is not a completely failsafe method but it should work in most cases
    INSTALL_DIR=`grep -E -i "^library_path=.*share/$VER_DIR" "$POVINI" | head -n 1 | sed "s?[^=]*=\"*??;s?/share/$VER_DIR.*??"`
    echo "$INSTALL_DIR"
  fi
}

# ==================================================================
#    Add file name to install log
# ==================================================================

log_install()
{
	if [ -w "$DEFAULT_DIR" ] ; then
		if [ ! -d "$DEFAULT_DIR/share/$VER_DIR/" ] ; then
			mkdir -p "$DEFAULT_DIR/share/$VER_DIR/" 2>&1 | tee -a "$TMP_LOG"
		fi
		LOG_NAME="$DEFAULT_DIR/share/$VER_DIR/install.log"
	else
		if [ ! -d "$HOME/.$POV_DIR/$VERSION/" ] ; then
			mkdir -p "$HOME/.$POV_DIR/$VERSION/" 2>&1 | tee -a "$TMP_LOG"
		fi
		if [ -w "$HOME/.$POV_DIR/$VERSION/" ] ; then
			LOG_NAME="$HOME/.$POV_DIR/$VERSION/install.log"
		else
			echo "could not write install log - check permissions!" | tee -a "$TMP_LOG"
			return 0
		fi
	fi

	if [ -z "$1$2" ] ; then
		echo "clearing install log." | tee -a "$TMP_LOG"
		rm -f "$LOG_NAME"
	fi

	if [ ! -r "$LOG_NAME" ] ; then
		echo "# $POVRAY version $VERSION_FULL install log" > "$LOG_NAME"
		echo "# started `date`" >> "$LOG_NAME"
	fi

	if [ "$1" = "B" ] ; then
		FILE_NAME=`echo "$2" | sed "s? ?%?g"`
		FILE_NAME="$FILE_NAME $3"
	else 
		FILE_NAME=`echo "$2" | sed "s? ?%?g"`
	fi

	echo "$1 $FILE_NAME" >> "$LOG_NAME"
}

# ==================================================================
#    install KDE icons and file types (system wide if possible)
# ==================================================================

kde_install()
{
  KDECONFIG=kde${KDE_SESSION_VERSION}-config

  if [ -z "$KDECONFIG" ] ; then
    return 0
  fi

  if [ -z "$1" ] ; then
    INSTALL_DIR=`install_dir`
  else
    INSTALL_DIR="$1"
  fi

  if [ -z "$INSTALL_DIR" ] ; then
    error_message "KDE integration NOT successful.  The directory 
where $POVRAY is installed could not be determined.
Make sure $POVRAY is correctly installed 
on this computer."
    return 0
  fi

  echo "------------------------------------------------------" 2>&1 | tee -a "$TMP_LOG"
  echo "installing basic KDE integration..." 2>&1 | tee -a "$TMP_LOG"

  KDE_INSTALL_DIR=`$KDECONFIG --prefix`
  if [ -z "$KDE_INSTALL_DIR" ] ; then
    error_message "KDE integration NOT successful.  
KDE installation on this computer does not exist or seems incomplete."
    return 0
  else
    if [ ! -w "$KDE_INSTALL_DIR" ] ; then
      if [ -z "$KDEHOME" ] ; then 
        if [ -d "$HOME/.kde" ] ; then 
          KDEHOME="$HOME/.kde"
        else
          error_message "could not determine user KDEHOME directory.
Make sure KDE is correctly installed"
          return 0
        fi
      else
        if [ ! -d "$KDEHOME" ] ; then
          error_message "user KDEHOME directory ($KDEHOME) does not exist"
          return 0
        fi
      fi
      if [ ! -w "$KDEHOME" ] ; then
        error_message "no write permission for user KDEHOME directory ($KDEHOME)"
        return 0
      fi
      KDE_INSTALL_DIR="$KDEHOME"
      echo "  kde integration for user $USER" 2>&1 | tee -a "$TMP_LOG"
    else
      echo "  kde integration system wide" 2>&1 | tee -a "$TMP_LOG"
    fi
  fi

  test -d "$KDE_INSTALL_DIR/share" || mkdir "$KDE_INSTALL_DIR/share" 2>&1 | tee -a "$TMP_LOG"

  if [ -d "$INSTALL_DIR/share/$VER_DIR/icons" ] ; then

    echo "  copying $POVRAY icons..." 2>&1 | tee -a "$TMP_LOG"

    test -d "$KDE_INSTALL_DIR/share/icons" || mkdir "$KDE_INSTALL_DIR/share/icons" 2>&1 | tee -a "$TMP_LOG"

		ICON_SETS="hicolor crystalsvg slick"

		for ICON_SET in $ICON_SETS ; do

			ICON_PREFIX="$KDE_INSTALL_DIR/share/icons/$ICON_SET"

			case $ICON_SET in
				"hicolor")
					ICON_SET_INTERN="classic"
					;;
				"crystalsvg")
					ICON_SET_INTERN="crystal"
					;;
				"slick")
					ICON_SET_INTERN="slick"
					;;
			esac

			test -d "$ICON_PREFIX" || mkdir "$ICON_PREFIX" 2>&1 | tee -a "$TMP_LOG"

			ICON_SIZES="16 32 48 64"

			for ICON_SIZE in $ICON_SIZES ; do

				test -d "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}" || mkdir "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}" 2>&1 | tee -a "$TMP_LOG"
				test -d "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}/mimetypes" || mkdir "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}/mimetypes" 2>&1 | tee -a "$TMP_LOG"

				if [ "$ICON_SET" = "hicolor" ] ; then
					test -d "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}/apps" || mkdir "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}/apps" 2>&1 | tee -a "$TMP_LOG"
					cp -f "$INSTALL_DIR/share/$VER_DIR/icons/povray_${ICON_SIZE}.png" "$KDE_INSTALL_DIR/share/icons/hicolor/${ICON_SIZE}x${ICON_SIZE}/apps/povray.png" 2>&1 | tee -a "$TMP_LOG"
					log_install "F" "$KDE_INSTALL_DIR/share/icons/hicolor/${ICON_SIZE}x${ICON_SIZE}/apps/povray.png"
				fi

				cp -f "$INSTALL_DIR/share/$VER_DIR/icons/file_pov_${ICON_SET_INTERN}_${ICON_SIZE}.png" "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}/mimetypes/povsdl_pov.png" 2>&1 | tee -a "$TMP_LOG"
				cp -f "$INSTALL_DIR/share/$VER_DIR/icons/file_inc_${ICON_SET_INTERN}_${ICON_SIZE}.png" "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}/mimetypes/povsdl_inc.png" 2>&1 | tee -a "$TMP_LOG"
				log_install "F" "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}/mimetypes/povsdl_pov.png"
				log_install "F" "$ICON_PREFIX/${ICON_SIZE}x${ICON_SIZE}/mimetypes/povsdl_inc.png"

			done

		done

    ICON_FILE="povray.png"

    echo "  generating $POVRAY file types..." 2>&1 | tee -a "$TMP_LOG"

    if [ ! -d "$KDE_INSTALL_DIR/share/mimelnk/text" ]; then
      mkdir -p "$KDE_INSTALL_DIR/share/mimelnk/text"
    fi

    echo "[Desktop Entry]
Comment=POV-Ray script file
Icon=povsdl_pov
Type=MimeType
MimeType=text/x-povray-script
Patterns=*.pov;*.POV;
" > "$KDE_INSTALL_DIR/share/mimelnk/text/x-povray-script.desktop"

		log_install "F" "$KDE_INSTALL_DIR/share/mimelnk/text/x-povray-script.desktop"

    echo "[Desktop Entry]
Comment=POV-Ray include file
Icon=povsdl_inc
Type=MimeType
MimeType=text/x-povray-include
Patterns=*.inc;*.INC;
" > "$KDE_INSTALL_DIR/share/mimelnk/text/x-povray-include.desktop"

		log_install "F" "$KDE_INSTALL_DIR/share/mimelnk/text/x-povray-include.desktop"

  else

    error_message "Could not find required files, make sure $POVRAY $VERSION_FULL is correctly installed
"

  fi

  echo "Finished basic KDE integration" 2>&1 | tee -a "$TMP_LOG"
  echo "------------------------------------------------------" 2>&1 | tee -a "$TMP_LOG"
  echo "" 2>&1 | tee -a "$TMP_LOG"

  return 1

}

# ==================================================================
#    install KDE panel entries
# ==================================================================

kde_install_user()
{
  if [ -z "$1" ] ; then
    INSTALL_DIR=`install_dir`
  else
    INSTALL_DIR="$1"
  fi

  if [ -z "$INSTALL_DIR" ] ; then
    error_message "KDE integration NOT successful.  The directory 
where $POVRAY is installed could not be determined.
Make sure $POVRAY is correctly installed 
on this computer."
    return 0
  fi

  if [ -z "$KDEHOME" ] ; then 
    if [ -d "$HOME/.kde" ] ; then 
      KDEHOME="$HOME/.kde"
    else
      error_message "could not determine user KDEHOME directory.  
Make sure KDE is correctly installed"
      return 0
    fi
  else
    if [ ! -d "$KDEHOME" ] ; then
      error_message "user KDEHOME directory ($KDEHOME) does not exist"
      return 0
    fi
  fi
  if [ ! -w "$KDEHOME" ] ; then
    error_message "no write permission for user KDEHOME directory ($KDEHOME)"
    return 0
  fi

  echo "------------------------------------------------------" 2>&1 | tee -a "$TMP_LOG"
  echo "installing KDE integration for user '$USER'..." 2>&1 | tee -a "$TMP_LOG"
  echo "  installing main $POVRAY $VERSION_FULL submenu..." 2>&1 | tee -a "$TMP_LOG"

  if [ ! -d "$KDEHOME/share/applnk" ] ; then
    mkdir -p "$KDEHOME/share/applnk" 2>&1 | tee -a "$TMP_LOG"
    KDE_PANEL_DIR="$KDEHOME/share/applnk/$VER_DIR"
  else
    KDE_PANEL_DIR="$KDEHOME/share/applnk/$VER_DIR"
  fi

  if [ -d "$KDE_PANEL_DIR" ] ; then 
    rm -rf $KDE_PANEL_DIR/* 2>&1 | tee -a "$TMP_LOG"
  else
    mkdir "$KDE_PANEL_DIR" 2>&1 | tee -a "$TMP_LOG"
  fi

  log_install "F" "$KDE_PANEL_DIR"

  echo "[Desktop Entry]
Name=POV-Ray $VERSION_FULL
Icon=povray
" > "$KDE_PANEL_DIR/.directory"

  echo "  installing ini file link..." 2>&1 | tee -a "$TMP_LOG"

  if [ -f "$HOME/.povray/$VERSION/povray.ini" ] ; then
    POVINI="$HOME/.povray/$VERSION/povray.ini"
    echo "[Desktop Entry]
Type=Application
Exec=kwrite $POVINI
Icon=txt
Name=edit user povray ini file (~/.povray/$VERSION/povray.ini)
" > "$KDE_PANEL_DIR/ini.desktop"
  else
    POVINI="$SYSCONFDIR/povray/$VERSION/povray.ini"
    echo "[Desktop Entry]
Type=Application
Exec=kdesu kwrite $POVINI
Icon=txt
Name=edit global povray.ini file
" > "$KDE_PANEL_DIR/ini.desktop"
  fi

  echo "  installing configuration file link..." 2>&1 | tee -a "$TMP_LOG"

  if [ -f "$HOME/.povray/$VERSION/povray.conf" ] ; then
    POVCONF="$HOME/.povray/$VERSION/povray.conf"
    echo "[Desktop Entry]
Type=Application
Exec=kwrite $POVCONF
Icon=txt
Name=edit IO-restrictions configuration file (~/.povray/$VERSION/povray.conf)
" > "$KDE_PANEL_DIR/conf_user.desktop"
  fi

  if [ -f "$SYSCONFDIR/povray/$VERSION/povray.conf" ] ; then
    POVCONF="$SYSCONFDIR/povray/$VERSION/povray.conf"
    echo "[Desktop Entry]
Type=Application
Exec=kdesu kwrite $POVCONF
Icon=txt
Name=edit global IO-restrictions configuration file
" > "$KDE_PANEL_DIR/conf_sys.desktop"
  fi

  echo "  installing documentation link..." 2>&1 | tee -a "$TMP_LOG"

  echo "[Desktop Entry]
Type=Application
Exec=konqueror $INSTALL_DIR/share/doc/$VER_DIR/html/index.html
Icon=html
Name=Documentation
" > "$KDE_PANEL_DIR/docu.desktop"

  echo "[Desktop Entry]
Type=Application
Exec=konqueror $INSTALL_DIR/share/doc/$VER_DIR/html/povlegal.html
Icon=html
Name=The POV-Ray licence (POVLEGAL.DOC)
" > "$KDE_PANEL_DIR/povlegal.desktop"

  echo "[Desktop Entry]
Type=Application
Exec=povray -benchmark ; read
Icon=exec
Name=run benchmark
Terminal=1
" > "$KDE_PANEL_DIR/benchmark.desktop"

  if [ -d "$INSTALL_DIR/share/$VER_DIR/scripts/" ] ; then

    echo "  installing sample scene render links..." 2>&1 | tee -a "$TMP_LOG"

    if [ -w "$INSTALL_DIR/share/$VER_DIR" ] ; then 
      SAMPLE_RESULTS_DIR="$INSTALL_DIR/share/$VER_DIR"
    else
      SAMPLE_RESULTS_DIR="$HOME/$VER_DIR"
      test -d "$SAMPLE_RESULTS_DIR" || mkdir "$SAMPLE_RESULTS_DIR"
      echo "This directory is generated by the $POVRAY $VERSION_FULL install script 
to contain the sample scene renders generated by the corresponding
entries in the KDE panel menu." > "$SAMPLE_RESULTS_DIR/README" 
    fi

    test -d "$SAMPLE_RESULTS_DIR/samples" || mkdir "$SAMPLE_RESULTS_DIR/samples"
    test -d "$SAMPLE_RESULTS_DIR/portfolio" || mkdir "$SAMPLE_RESULTS_DIR/portfolio"

    echo "Here you can find the rendered sample animations after running the sample animations render script" > "$SAMPLE_RESULTS_DIR/samples/animations.html"
    echo "Here you can find the rendered sample scenes after running the sample scenes render script" > "$SAMPLE_RESULTS_DIR/samples/stills.html"
    echo "Here you can find the portfolio after running the portfolio render script" > "$SAMPLE_RESULTS_DIR/portfolio/index.html"

    if [ -d "$KDE_PANEL_DIR/samples" ] ; then 
      rm -rf $KDE_PANEL_DIR/samples* 2>&1 | tee -a "$TMP_LOG"
    else
      mkdir "$KDE_PANEL_DIR/samples" 2>&1 | tee -a "$TMP_LOG"
    fi

    echo "[Desktop Entry]
Name=Sample scenes
Icon=folder
" > "$KDE_PANEL_DIR/samples/.directory"

    echo "[Desktop Entry]
Type=Application
Exec=$INSTALL_DIR/share/$VER_DIR/scripts/allscene.sh -o $SAMPLE_RESULTS_DIR/samples -h $SAMPLE_RESULTS_DIR/samples/stills.html
Icon=exec
Name=render sample scenes (stills)
Terminal=1
" > "$KDE_PANEL_DIR/samples/render_stills.desktop"

    echo "[Desktop Entry]
Type=Application
Exec=$INSTALL_DIR/share/$VER_DIR/scripts/allanim.sh -o $SAMPLE_RESULTS_DIR/samples -h $SAMPLE_RESULTS_DIR/samples/animations.html
Icon=exec
Name=render sample scenes (animations)
Terminal=1
" > "$KDE_PANEL_DIR/samples/render_animations.desktop"

    echo "[Desktop Entry]
Type=Application
Exec=$INSTALL_DIR/share/$VER_DIR/scripts/portfolio.sh -o $SAMPLE_RESULTS_DIR/portfolio
Icon=exec
Name=render portfolio
Terminal=1
" > "$KDE_PANEL_DIR/samples/render_portfolio.desktop"

    echo "[Desktop Entry]
Type=Application
Exec=konqueror $SAMPLE_RESULTS_DIR/portfolio/index.html
Icon=html
Name=View portfolio
" > "$KDE_PANEL_DIR/samples/view_portfolio.desktop"

    echo "[Desktop Entry]
Type=Application
Exec=konqueror $SAMPLE_RESULTS_DIR/samples/animations.html
Icon=imagegallery
Name=Sample scene gallery (animations)
" > "$KDE_PANEL_DIR/samples/animations.desktop"

    echo "[Desktop Entry]
Type=Application
Exec=konqueror $SAMPLE_RESULTS_DIR/samples/stills.html
Icon=imagegallery
Name=Sample scene gallery (stills)
" > "$KDE_PANEL_DIR/samples/stills.desktop"

    echo "  modifying povray.conf..." 2>&1 | tee -a "$TMP_LOG"
		add_readwrite_conf "$SAMPLE_RESULTS_DIR"

  else

    error_message "Could not find required files, make sure $POVRAY $VERSION_FULL is correctly installed
"
  fi

  # needs an extra invitation
	if [ -d "$KDEHOME/share/applnk-redhat" ] ; then 
		KDE_RH_PANEL_DIR="$KDEHOME/share/applnk-redhat/$VER_DIR"
		if [ -L "$KDE_RH_PANEL_DIR" ] ; then 
			rm "$KDE_RH_PANEL_DIR" 2>&1 | tee -a "$TMP_LOG"
		fi
		if [ -d "$KDE_RH_PANEL_DIR" ] ; then 
			rm -rf "$KDE_RH_PANEL_DIR" 2>&1 | tee -a "$TMP_LOG"
		fi
		ln -s "$KDE_PANEL_DIR" "$KDE_RH_PANEL_DIR" 2>&1 | tee -a "$TMP_LOG"
		log_install "F" "$KDE_RH_PANEL_DIR"
  fi

  echo "Finished installing KDE panel entries" 2>&1 | tee -a "$TMP_LOG"
  echo "------------------------------------------------------" 2>&1 | tee -a "$TMP_LOG"
  echo "" 2>&1 | tee -a "$TMP_LOG"

  return 1
}

# @@KDE_END@@


kde_install

