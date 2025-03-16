echo "##########################################################"
echo "#             Compiling OPENGL Testing....               #"
echo "##########################################################"

clang -std=c99 -o bin/coordEx src/coordinate.c -I/opt/homebrew/include -L/opt/homebrew/lib -Iinclude -lglfw -framework Cocoa -framework OpenGL -framework IOKit -DGL_SILENCE_DEPRECATION

if [ $? -eq 0 ]; then
    echo "Compile complete...."
    echo "##########################################################"
    echo "#             Running OPENGL Testing....                 #"
    echo "##########################################################"
    bin/coordEx

else
    echo "Compilation failed."
    exit 1
fi

