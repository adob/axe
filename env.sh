
path="$PWD":"$PWD/bin"


if [[ $PATH != *$path* ]]
then
    OLDPATH="$PATH"
    PATH="$path":"$PATH"
else
    echo "skipping"
fi

export ASAN_OPTIONS=handle_segv=0
