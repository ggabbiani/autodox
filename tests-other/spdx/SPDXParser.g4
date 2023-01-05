/*
 * SPDX parser grammar build from [Annex D: SPDX License Expressions](https://spdx.github.io/spdx-spec/v2-draft/SPDX-license-expressions/)
 *
 * SPDX-FileCopyrightText: Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

parser grammar SPDXParser;

options { 
    tokenVocab = SPDXLexer;
}

all     : license_expression? EOF;

license_expression  
    : simple_expression
    | compound_expression
    ;

simple_expression   
    : license_and_beyond
    | LICENSE_REF
    ;

compound_expression
    : simple_expression                                 
    | simple_expression     op=WITH xid=ID              
    | compound_expression   op=AND  compound_expression 
    | compound_expression   op=OR   compound_expression 
    | LEFT_PAREN compound_expression RIGHT_PAREN
    ;

license_and_beyond  : id=ID op=PLUS?;