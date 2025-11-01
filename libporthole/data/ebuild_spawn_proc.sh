#!/bin/bash
ROOT=$0

while read -r line; do
	if [[ "$line" = "<<<byebye!>>>" ]]; then
		# echo "bye bye ;-/"
		exit 0
	fi
	source "${ROOT}/${line}.ebuild"

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

	echo "${EAPI}"
	echo "${DESCRIPTION}"
	echo "${BDEPEND}"
	echo "${HOMEPAGE}"
	echo "${IDEPEND}"
	echo "${INHERIT}"
	echo "${IUSE}"
	echo "${KEYWORDS}"
	echo "${LICENSE}"
	echo "${RDEPEND}"
	echo "${REQUIRED_USE}"
	echo "${RESTRICT}"
	echo "${SLOT}"
	echo -n "${SRC_URI}"
	printf "\0"
	#printf "${EAPI}\n${DESCRIPTION}\n${BDEPEND}\n${HOMEPAGE}\n${IDEPEND}\n${INHERIT}\n${IUSE}\n${KEYWORDS}\n${LICENSE}\n${RDEPEND}\n${REQUIRED_USE}\n${RESTRICT}\n${SLOT}\n${SRC_URI}\0"
done

exit 1
