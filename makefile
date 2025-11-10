.PHONY: all clean 
# эти цели не являются файлами выполняй их даже если соотв файлы существуют
all: akinator
# когда запускаем make без цели, то выполняем первую цель после all, то есть записи make stack make all и make эквивалентны

COMP=clang++

#make DEBUG=1

CFLAGS_DEBUG = -D _DEBUG 
COMMON_CFLAGS = -ggdb3 -std=c++20 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla -Wno-c++11-extensions
# CFLAGS = -D _DEBUG


CFLAGS = $(COMMON_CFLAGS) $(CFLAGS_DEBUG)

%.o: %.cpp
	$(COMP) -c $< -o $@ $(CFLAGS) 
# clang++ -cpp main.cpp (зависимость) -o main.o (цель) флаги

akinator: main.o akinator.o tree_dump.o
	$(COMP) -o $@ $^ 

# $@ имя цели
# $^ все зависимости
# $(COMP) clang++
# clang++ -o stack main.o hash.o log.o my_assert.o stack_func.o

clean:
	rm -f tree *.o images/*.dot images/*.svg
# убирать исполняемый файл
# make -C tree_akinator akinator
