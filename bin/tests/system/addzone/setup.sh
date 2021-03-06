#!/bin/sh
#
# Copyright (C) 2010, 2012-2014, 2016, 2017  Internet Systems Consortium, Inc. ("ISC")
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

SYSTEMTESTTOP=..
. $SYSTEMTESTTOP/conf.sh

$SHELL clean.sh

cp -f ns1/redirect.db.1 ns1/redirect.db
cp -f ns2/redirect.db.1 ns2/redirect.db
cp -f ns3/redirect.db.1 ns3/redirect.db
cp -f ns2/named1.conf ns2/named.conf
cp -f ns2/default.nzf.in ns2/3bf305731dd26307.nzf
mkdir ns2/new-zones
