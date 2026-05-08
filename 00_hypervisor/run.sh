#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODULE_NAME="hyper_box"
KO_PATH="${SCRIPT_DIR}/${MODULE_NAME}.ko"

cd "${SCRIPT_DIR}"

if command -v rg >/dev/null 2>&1; then
  LOG_FILTER=(rg)
else
  LOG_FILTER=(grep -F)
fi

if [[ "${EUID}" -ne 0 ]]; then
  if ! sudo -n true 2>/dev/null; then
    echo "[run.sh] root 권한이 필요합니다."
    echo "[run.sh] 아래처럼 실행해주세요:"
    echo "  sudo ./run.sh"
    exit 1
  fi
  SUDO="sudo -n"
else
  SUDO=""
fi

if [[ ! -f "${KO_PATH}" ]]; then
  echo "[run.sh] ${KO_PATH} not found. Building..."
  make
fi

is_loaded() {
  ${SUDO} lsmod | grep -q "^${MODULE_NAME}\\b"
}

show_module_holders() {
  local base="/sys/module/${MODULE_NAME}"
  if [[ -r "${base}/refcnt" ]]; then
    echo "[run.sh] ${MODULE_NAME} refcnt: $(cat "${base}/refcnt")"
  fi
  if [[ -d "${base}/holders" ]]; then
    local holders
    holders="$(ls -1 "${base}/holders" 2>/dev/null || true)"
    if [[ -n "${holders}" ]]; then
      echo "[run.sh] ${MODULE_NAME} holders:"
      echo "${holders}" | sed 's/^/[run.sh]   - /'
    else
      echo "[run.sh] ${MODULE_NAME} holders: (none)"
    fi
  fi
}

unload_module() {
  if is_loaded; then
    echo "[run.sh] Removing ${MODULE_NAME}"
    if ! ${SUDO} rmmod "${MODULE_NAME}"; then
      echo "[run.sh] rmmod 실패: Module is in use"
      show_module_holders
      return 1
    fi
  fi
}

trap 'unload_module || true' EXIT

if ${SUDO} lsmod | grep -q "^${MODULE_NAME}\\b"; then
  echo "[run.sh] ${MODULE_NAME} is already loaded. Removing first..."
  unload_module
fi

echo "[run.sh] Inserting ${KO_PATH}"
${SUDO} insmod "${KO_PATH}"

echo "[run.sh] Kernel log (matching '${MODULE_NAME}:')"
${SUDO} dmesg --color=never | "${LOG_FILTER[@]}" "${MODULE_NAME}:"

unload_module

echo "[run.sh] Kernel log after rmmod (matching '${MODULE_NAME}:')"
${SUDO} dmesg --color=never | "${LOG_FILTER[@]}" "${MODULE_NAME}:"
