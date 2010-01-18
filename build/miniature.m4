AC_DEFUN([MINIATURE_CHECK_PLATFORM],[
  AC_ARG_VAR([MINIATURE_PLATFORM], [target platform for Miniature])


  if test "auto" == "${MINIATURE_PLATFORM:-auto}"; then
    AC_MSG_CHECKING([target platform for Miniature])


    while true; do
      if test -f /etc/maemo_version; then
        maemo_version="`cat /etc/maemo_version`"

        case "$maemo_version" in
          5.*) MINIATURE_PLATFORM=maemo5;;
          6.*) MINIATURE_PLATFORM=maemo6;;
          *)   AC_MSG_RESULT([unsupported])
               AC_MSG_ERROR([Maemo $maemo_version is not supported yet]);;
        esac

        break
      fi

      MINIATURE_PLATFORM=desktop
      MINIATURE_PLATFORM_SOURCES=src/platforms/others.o
      break
    done
  fi

  AC_MSG_RESULT([$MINIATURE_PLATFORM])
  AM_CONDITIONAL([IS_MINIATURE_PLATFORM_MAEMO5], [test "${MINIATURE_PLATFORM}" == "maemo5"])
  AM_CONDITIONAL([IS_MINIATURE_PLATFORM_MAEMO6], [test "${MINIATURE_PLATFORM}" == "maemo6"])

  case "$MINIATURE_PLATFORM" in
    maemo5) test -z "$QT_PATH" && QT_PATH=/opt/qt4-maemo5/bin;;
  esac
])
