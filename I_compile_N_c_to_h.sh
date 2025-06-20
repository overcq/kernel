#!/bin/sh
################################################################################
#   ___   publicplace
#  ¦OUX¦  ‘unix’ “sh” environment
#  ¦/C+¦  OUX/C+ OS
#   ---   kernel
#         “.c” to “.h” utility for structure fragments from a source file written in conformance to the strict syntax style
# ©overcq                on ‟Gentoo Linux 13.0” “x86_64”             2015‒1‒13 #
################################################################################
case "$1" in
-f)
    echo enum
    perl -e '
        use strict;
        use warnings;
        sub enc
        {   my $file_identifier = shift;
            $file_identifier =~ s`_`_5f`g;
            $file_identifier =~ s`[^_0-9A-Za-z]` '\''_'\''. sprintf( '\''%02x'\'', ord( $& )) `eg;
            return $file_identifier;
        }
        local $\ = $/;
        print '\''{ _F_uid( '\''. enc( shift @ARGV ) .'\'' ) = ( ~0 << ( sizeof(int) * 8 / 2 - 1 )) | 1'\'';
        while( $_ = shift @ARGV )
        {   print '\'', _F_uid( '\''. enc( $_ ) .'\'' )'\'';
        }
        print '\''};'\'';
    ' "$@"
    ;;
-h1) # ‹report› ‘uid’ declarations
    trap 'rm "$tmp_file_0" "$tmp_file_1" "$tmp_file_2"' EXIT
    tmp_file_0=$(mktemp); tmp_file_1=$(mktemp); tmp_file_2=$(mktemp)
    awk '
        /\/\*/,/\*\// {
            print ""
            next
        }
        { print }
    ' < "$2" \
    | perl -ne '
        use strict;
        use warnings;
        local $\ = $/;
        chomp;
        /\bX_B\(\s*(\w+)\s*,\s*(\w+)\s*,/ and print "$1,X_$2";
        /\bYi_B\(\s*(\w+)\s*,\s*(\w+)\s*\)/ and print "$1,Yi_$2";
    ' \
    | sort -u \
    > "$tmp_file_1"
    if [ -s "$tmp_file_1" ]; then
        if [ -n "$3" ]; then
            awk '
                /\/\*/,/\*\// {
                    print ""
                    next
                }
                { print }
            ' < "$3" \
            | perl -ne '
                use strict;
                use warnings;
                local $\ = $/;
                chomp;
                /^[,{] _XhYi_uid\((\w+),(\w+)\)/ and print "$1,$2";
            ' \
            > "$tmp_file_2"
            sort "$tmp_file_1" "$tmp_file_2" \
            | uniq -u \
            > "$tmp_file_0"
            sort "$tmp_file_1" "$tmp_file_0" \
            | uniq -d \
            > "$tmp_file_2"
            s="$tmp_file_2"
            tmp_file_2="$tmp_file_1"
            tmp_file_1="$s"
        fi
        if [ -z "$3" -o -s "$tmp_file_1" ]; then
            echo 'enum'
            perl -e '
                use strict;
                use warnings;
                binmode STDIN, ":bytes";
                my $file_identifier = $ARGV[0];
                if( $file_identifier =~ m`/module(?:/[^/]+){2}$` )
                {   $file_identifier =~ s`.*/([^/]+/)`$1`;
                }else
                {   $file_identifier =~ s`.*/``;
                }
                $file_identifier =~ s`\.c$``;
                $file_identifier =~ s`_`_5f`g;
                $file_identifier =~ s`[^_0-9A-Za-z]` '\''_'\''. sprintf( '\''%02x'\'', ord( $& )) `eg;
                local $\ = $/;
                $_ = <STDIN>;
                chomp;
                print "{ _XhYi_uid($_) = _XhYi_F_uid( ${file_identifier} )";
                while(<STDIN>)
                {   chomp;
                    print ", _XhYi_uid($_)";
                }
                print '\''};'\''
            ' "$2" < "$tmp_file_1"
        fi
    fi
    ;;
-h2) # type forward declarations
    perl -e '
        use strict;
        use warnings;
        local $\ = $/;
        while(<>)
        {   chomp;
            s`//.*$``;
            if( /^\s*#(?:if|(?:elif|else|endif)\b)/ )
            {   print;
            }elsif( /^(?:struct|union)\s+E_\w+/ ) # typy publiczne
            {   print "$&;";
            }
        }
    ' "$2"
    ;;
-h3) # type definitions, variable and procedure forward declarations
    perl -e '
        use strict;
        use warnings;
        my $extern = 0;
        my $inside_braces = 0;
        my $last_line;
        local $\ = $/;
        while(<>)
        {   chomp;
            s`//.*$``;
            if( $inside_braces == 0
            and defined $last_line
            )
            {   if( $last_line =~ /^(?:_export|_private)$/ )
                {   $extern = 1;
                }elsif( $last_line =~ /;/ )
                {   $extern = 0;
                }
            }
            if( $inside_braces == 1 )
            {   if( /^{/ )
                {   print $last_line;
                    print;
                    $inside_braces = 2;
                    next;
                }
                $inside_braces = 0;
            }
            if( $inside_braces == 2 )
            {   if( /^}[^=;]*/ )
                {   my $s = $&;
                    if( /^}\s*(\*)*\w/ )
                    {   print '\''}extern'\''. ( !defined( $1 ) ? '\'\ \'' : '\'\'' ) . substr( $s, 1 ) .'\'';'\'';
                    }else
                    {   print $s .'\'';'\'';
                    }
                    $inside_braces = /;/ ? 0 : 3;
                    $extern = 0;
                }else
                {   print;
                }
                next;
            }
            if( $inside_braces == 3 )
            {   $inside_braces = 0 if /^}/;
                next;
            }
            if( $inside_braces == 4 )
            {   if( /^\)/ )
                {   print $& .'\'';'\'';
                    $inside_braces = 0;
                    $extern = 0;
                }else
                {   print;
                }
                next;
            }
            if( /^\s*#(?:if|(?:elif|else|endif)\b)/ )
            {   print;
            }elsif( /^(?:_export|_private)\s+(?:\w+\s+)*?((?:(?:enum|struct|union)\s+)?.*?\b\(?\**E_[^=;]*)[=;]/ ) # zmienne publiczne
            {   print "extern $1;";
            }elsif( /^(?:enum|struct|union)\s+.*?\bE_\w+/ ) # typy publiczne
            {   $inside_braces = 1;
                $last_line = $_;
            }elsif( $extern and /^E_\w+\(/ ) # procedura publiczna
            {   print $last_line;
                print;
                $inside_braces = 4;
            }elsif( /^D(\([^)]*\))/ ) # ‹zadanie›
            {   print "extern I D_id$1; $&;";
            }else
            {   $last_line = $_;
            }
        }
    ' "$2"
    ;;
-c)
    perl -e '
        use strict;
        use warnings;
        my $extern = 0;
        my $inside_comment = 0;
        my $inside_braces = 0;
        my $inside_enum;
        my $last_line;
        local $\ = $/;
        sub print1
        {   print $last_line if defined $last_line;
            $last_line = shift;
            $last_line = $_ unless defined $last_line;
        }
        sub print_blank
        {   print $last_line if defined $last_line;
            $last_line = "";
        }
        while(<>)
        {   chomp;
            if( $inside_comment )
            {   if( /\*\// )
                {   $_ = $'\'';
                    $inside_comment = 0;
                }else
                {   print_blank;
                    next;
                }
            }
            s`//.*$``;
            if( $inside_braces == 0
            and defined $last_line
            )
            {   if( $last_line =~ /^(?:_export|_private)$/ )
                {   $extern = 1;
                }elsif( $last_line =~ /;/ )
                {   $extern = 0;
                }
            }
            if( $inside_braces == 1 )
            {   if( /^{/ )
                {   print_blank;
                    $inside_braces = 2;
                    next;
                }
                $inside_braces = 0;
            }
            if( $inside_braces == 2 )
            {   if( /^}/ )
                {   unless( $inside_enum )
                    {   $inside_braces = /;/ ? 0 : 3;
                        s`^.``;
                        print1;
                        $extern = 0;
                    }else
                    {   $inside_braces = 0;
                        print_blank;
                    }
                }else
                {   print_blank;
                }
                next;
            }
            if( $inside_braces == 3 )
            {   print1;
                $inside_braces = 0 if /^}/;
                next;
            }
            if( $inside_braces == 4 )
            {   if( /^\)/ )
                {   print1;
                    $inside_braces = 0;
                    $extern = 0;
                }else
                {   print1;
                }
                next;
            }
            if( /^(?:_export|_private)\s+(?:\w+\s+)*?(?:(?:enum|struct|union)\s+)?.*?\b\**E_.*;/ ) # zmienne publiczne
            {   print1;
            }elsif( /^(?:enum|struct|union)\s+.*?\bE_\w/ ) # typy publiczne
            {   $inside_enum = /^enum/;
                unless( $inside_enum )
                {   print1;
                }else
                {   print_blank;
                }
                $inside_braces = 1;
            }elsif( $extern and /^E_\w+\(/ ) # procedura publiczna
            {   print1;
                $inside_braces = 4;
            }elsif( /^D(\([^)]*\))/ ) # ‹zadanie›
            {   print1 "_private I D_id$1 = ~0; $_";
            }elsif( /^\/\*/ ) # komentarz blokowy
            {   if( /\*\// )
                {   $_ = $'\'';
                    redo;
                }
                print_blank;
                $inside_comment = 1
            }else
            {   print1;
            }
        }
        print $last_line if defined $last_line;
    ' "$2"
    ;;
esac
################################################################################
