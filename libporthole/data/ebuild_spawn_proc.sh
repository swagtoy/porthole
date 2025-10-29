#!/bin/bash
ROOT=$0

function inherit() {
	:;
}

while read -r line; do
	if [[ "$line" = "<<<byebye!>>>" ]]; then
		echo "bye bye ;-/"
		exit 0
	fi
	source "$ROOT/$line"
	DESCRIPTION="${DESCRIPTION//$'\n'/ }"
	BDEPEND="${BDEPEND//$'\n'/ }"
	HOMEPAGE="${HOMEPAGE//$'\n'/ }"
	IDEPEND="${IDEPEND//$'\n'/ }"
	INHERIT="${INHERIT//$'\n'/ }"
	IUSE="${IUSE//$'\n'/ }"
	KEYWORDS="${KEYWORDS//$'\n'/ }"
	LICENSE="${LICENSE//$'\n'/ }"
	RDEPEND="${RDEPEND//$'\n'/ }"
	REQUIRED_USE="${REQUIRED_USE//$'\n'/ }"
	RESTRICT="${RESTRICT//$'\n'/ }"
	SLOT="${SLOT//$'\n'/ }"
	SRC_URI="${SRC_URI//$'\n'/ }"
	#printf "Processing: $ROOT/$line\ntest\n\nhello"
	printf "${EAPI}\n${DESCRIPTION}\n${BDEPEND}\n${HOMEPAGE}\n${IDEPEND}\n${INHERIT}\n${IUSE}\n${KEYWORDS}\n${LICENSE}\n${RDEPEND}\n${REQUIRED_USE}\n${RESTRICT}\n${SLOT}\n${SRC_URI}\0"
done

exit 1
