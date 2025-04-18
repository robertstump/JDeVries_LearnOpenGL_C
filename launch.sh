#Consider flag for turning off testing, or turning off fresh compile to just launch etc.
#
echo "##########################################################"
echo "#             Compiling OPENGL Testing....               #"
echo "##########################################################"

DEBUG_FLAGS="-fsanitize=address -g -O0 -Wall -Werror"
LIGHT_DBG_FLAGS="-g -Wall -Werror"
CFLAGS=$LIGHT_DBG_FLAGS
TEST_ONLY=false

#for arg in "$@"; do
    #if [ "$arg" = "-d" ] || [ "$arg" = "--debug" ]; then
        #echo "[!] Debug mode enabled."
        #CFLAGS=$DEBUG_FLAGS
    #fi  
#done

for arg in "$@"; do 
    case "$arg" in 
        -d|--debug)
            echo "[!] Debug mode enable."
            CFLAGS=$DEBUG_FLAGS
            ;;
        -t|--test-only)
            echo "[!] Running in test-only mode."
            TEST_ONLY=true;;
        *)
            echo "Syntax: launch.sh [flag][flag]"
            echo "Flags --debug, -d, --test-only, -t"
            ;;
    esac
done

if [ "$TEST_ONLY" = true ]; then
    echo
    echo "##########################################################"
    echo "#                   Running Unit Tests....               #"
    echo "##########################################################"

    if ! ./test.sh; then
        echo "[ ] Test compilation failed."
        exit 1
    fi

    if ! ./runtests.sh; then
        echo "[ ] Unit tests failed."
        exit 1
    fi

echo "[X] All tests passed."
exit 0

fi

SRC_DIR="src"
INCLUDE_DIR="include"
BIN_DIR="bin"
TARGET="$BIN_DIR/lighting4Ex_rotate"

SRC_MAIN="$SRC_DIR/lighting4_rotateLight.c"
#SRC_SECONDARY="SRC_DIR/..."

clang -std=c99 $LIGHT_DBG_FLAGS -o $TARGET $SRC_MAIN -I/opt/homebrew/include -L/opt/homebrew/lib -Iinclude -lglfw -framework Cocoa -framework OpenGL -framework IOKit -DGL_SILENCE_DEPRECATION

if [ $? -ne 0 ]; then
    echo "[ ] Compilation Failed."
    exit 1
fi

echo "[X] Main Compilation Complete"
echo 
echo "##########################################################"
echo "#                   Running Unit Tests....               #"
echo "##########################################################"

if ! ./test.sh; then
    echo "[ ] Test compilation failed."
    exit 1
fi

if ! ./runtests.sh; then
    echo "[ ] Unit tests failed."
    exit 1
fi

echo "[X] All tests passed."

echo "##########################################################"
echo "#             Running OPENGL Testing....                 #"
echo "##########################################################"
echo

./$TARGET
