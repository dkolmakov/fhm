/*
   Copyright 2019-2023 Dmitry Kolmakov

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include <string>

const char const_configuration[] =
    "\t# Create history manager home directory\n"
    "\tmkdir -p $hm_home\n"
    "\n"
    "\t# Path to the HM database\n"
    "\thm_history_db=$hm_home/history.db\n"
    "\n"
    "\t# Create a new session entry in the database corresponding to the "
    "current terminal session\n"
    "\thm_session_id=$(hm-db $hm_history_db session)\n"
    "\n"
    "\t# Set a script to be executed after each other command in the current "
    "terminal session\n"
    "\tprompt_command() {\n"
    "\t\trc=$?;\n"
    "\t\tlast=\"$(hm_extract_last_cmd)\";\n"
    "\t\tif [ $(id -u) -ne 0 ] && ! [ -z $hm_session_id ] && [ \"$last\" != "
    "\"$hm_last_command\" ];\n"
    "\t\tthen\n"
    "\t\t\thm-db $hm_history_db add $hm_session_id \"$(date +\"%Y-%m-%d "
    "%H:%M:%S\")\" $(pwd) \"$last\" $rc --async;\n"
    "\t\t\texport hm_last_command=$last\n"
    "\t\tfi\n"
    "\t}\n"
    "\n"
    "\t# Wrapper for hm-db which adds the selected commands at the beginning "
    "of the current terminal history\n"
    "\thm() {\n"
    "\t\t# Parse arguments\n"
    "\t\tARGS=\n"
    "\t\tfor i in \"$@\"\n"
    "\t\tdo \n"
    "\t\tcase $i in \n"
    "\t\t\t-v|--version)\n"
    "\t\t\thm-db --version\n"
    "\t\t\t;;\n"
    "\t\t\t-h|--help)\n"
    "\t\t\thm-db --wrapped-help\n"
    "\t\t\t;;\n"
    "\t\t\t-i|--info)\n"
    "\t\t\thm-db $hm_history_db info $hm_session_id\n"
    "\t\t\t;;\n"
    "\t\t\t*)\n"
    "\t\t\tARGS=\"$ARGS $i\"\n"
    "\t\t\t;;\n"
    "\t\tesac\n"
    "\t\tdone\n"
    "\n"
    "\t\t# Perform history recovery\n"
    "\t\tif ! [ -z \"$ARGS\" ]\n"
    "\t\tthen\n"
    "\t\t\tTMPFILE=$(mktemp extracted_history.XXXXXXXXXX)\n"
    "\t\t\t# Update history\n"
    "\t\t\thm-db $hm_history_db select $hm_session_id $(echo $ARGS) > $TMPFILE "
    "&& echo hm \"$*\" >> $TMPFILE && hm_load_history $TMPFILE\n"
    "\t\t\tif [ $? -eq 0 ]; then echo \"History successfully updated!\"; fi\n"
    "\t\t\trm -rf $TMPFILE\n"
    "\t\tfi\n"
    "\t}\n"
    "\n"
    "\tif [ $ZSH_VERSION ]\n"
    "\tthen\n"
    "\t\thm_load_history() {\n"
    "\t\t\tfc -R $1\n"
    "\t\t\tfc -W\n"
    "\t\t}\n"
    "\t\thm_extract_last_cmd() {\n"
    "\t\t\techo $(fc -ln -1)\n"
    "\t\t}\n"
    "\t\tprecmd_functions=(${precmd_functions} prompt_command)\n"
    "\tfi\n"
    "\n"
    "\tif [ $BASH_VERSION ]\n"
    "\tthen\n"
    "\t\thm_load_history() { \n"
    "\t\t\tSAVE_HISTFILE=$HISTFILE\n"
    "\t\t\tHISTFILE=$1\n"
    "\t\t\thistory -r\n"
    "\t\t\tHISTFILE=$SAVE_HISTFILE\n"
    "\t\t\thistory -w\n"
    "\t\t}\n"
    "\t\thm_extract_last_cmd() {\n"
    "\t\t\thistory 1 | sed -E \"s/^ *[0-9]* *//\"\n"
    "\t\t}\n"
    "\t\texport PROMPT_COMMAND='prompt_command;'$PROMPT_COMMAND\n"
    "\tfi\n";
