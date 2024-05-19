lint:
	cppcheck *.h --enable=warning,style,performance,portability --inline-suppr --language=c++
