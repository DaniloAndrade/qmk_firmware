#pragma once

/* Flow Tap: evita que mod-tap resolva como "hold" quando a tecla anterior
 * foi digitada rápido (ex: 'a' + 'space' em sequência de digitação normal) */
#define FLOW_TAP_TERM 150

/* Permissive Hold: combos legítimos entre mãos opostas (ex: A-hold + K)
 * resolvem como hold assim que a outra tecla é solta, sem esperar o
 * tapping term inteiro */
#define PERMISSIVE_HOLD
