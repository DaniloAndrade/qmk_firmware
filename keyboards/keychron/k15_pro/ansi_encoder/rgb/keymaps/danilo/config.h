#pragma once

/* Tapping Term: tempo que uma tecla mod-tap precisa ficar pressionada
 * sozinha (sem nenhuma outra tecla interromper) antes de virar "hold" por
 * timeout puro. Default do QMK é 200ms; subimos pra 250ms porque dedos
 * mais lentos (ex: mindinho) às vezes ultrapassam 200ms sem intenção de
 * segurar como modificador — Flow Tap/Chordal Hold não cobrem esse caso
 * porque não há tecla interrompendo para eles arbitrarem. */
#define TAPPING_TERM 250

/* Flow Tap: evita que mod-tap resolva como "hold" quando a tecla anterior
 * foi digitada rápido (ex: 'a' + 'space' em sequência de digitação normal) */
#define FLOW_TAP_TERM 150

/* Permissive Hold: combos legítimos entre mãos opostas (ex: A-hold + K)
 * resolvem como hold assim que a outra tecla é solta, sem esperar o
 * tapping term inteiro */
#define PERMISSIVE_HOLD

/* Chordal Hold: rede de segurança extra — só permite "hold" quando as duas
 * teclas envolvidas são de mãos opostas; combos na mesma mão (rolls) sempre
 * viram tap */
#define CHORDAL_HOLD
