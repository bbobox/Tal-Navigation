#!/bin/bash

yatea -rcfile /usr/local/etc/yatea/yatea.rc Corpustest.ttg ; iconv yatea.dtd > Corpustest/default/xml/yatea.dtd ;

exit 0