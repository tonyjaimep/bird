def Settings( **kwargs ):
    return {
        'flags': [ '-x', 'c++', '-Wall', '-Wextra', '-Werror', '-lsfml-graphics', '-lsfml-window', '-lsfml-system'],
    }
