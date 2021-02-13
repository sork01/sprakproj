.PHONY: bigrams gp2010 sbc

bigrams:
	cat sent6.txt | tr -cd '[:alnum:]ÅÄÖåäö ' | tr '[:upper:]' '[:lower:]' | tr 'ÅÄÖ' 'åäö' | tr '\n' '.' | sed 's/\./ . /g' | sed -r 's/ /\n/g' | sed -r 's/[0-9]+/NUMBER/g' > /tmp/left.txt
	echo '' > /tmp/right.txt
	cat /tmp/left.txt >> /tmp/right.txt
	paste -d ' ' /tmp/right.txt /tmp/left.txt | sort | uniq -c > /tmp/bigrams1.txt

gp2010:
	wget -O /tmp/gp2010.xml.bz2 'http://spraakbanken.gu.se/lb/resurser/meningsmangder/gp2010.xml.bz2'
	bunzip2 /tmp/gp2010.xml.bz2

sbc:
	./sbc.sh
