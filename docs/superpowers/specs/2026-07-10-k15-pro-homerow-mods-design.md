# K15 Pro RGB — Mitigar disparo acidental de mod-tap em digitação rápida

## Contexto e problema

O usuário customiza o Keychron K15 Pro RGB via **Keychron Launch** (app baseado no
protocolo VIA), usando home row mods no esquema GACS espelhado:

- Esquerda: `A`=GUI, `S`=Alt, `D`=Ctrl, `F`=Shift
- Direita: `J`=Shift, `K`=Ctrl, `L`=Alt, `;`=GUI

Ao digitar rápido, sequências como `a` seguido de `space` (ex: final de palavra
"a ") disparam o Spotlight do macOS — o firmware está resolvendo o mod-tap do
`A` como *hold* (GUI) em vez de *tap* (letra "a"), porque outra tecla (`space`)
foi pressionada durante a janela de tapping term. O mesmo ocorre com outras
teclas home-row customizadas.

O usuário sempre pressiona a tecla de espaço **esquerda** com o polegar
esquerdo (o K15 Pro tem duas teclas de espaço físicas, esquerda e direita).

Requisito importante: o combo GUI+Space deliberado (segurando `A` por mais
tempo antes de apertar Space) deve continuar funcionando — a correção deve
mirar especificamente o "rolling" de digitação rápida, não desabilitar o
combo.

## Restrição de projeto

Não alterar nada do que já existe no repositório (em especial
`keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/via/`). A correção deve
viver em uma keymap nova, e o firmware resultante deve continuar funcionando
normalmente com o Keychron Launch (VIA) para remapeamento de teclas — os
home row mods em si continuam sendo configurados dinamicamente pelo Launch,
não hard-coded na keymap.

## Pesquisa (resumo)

A comunidade QMK trata esse problema como "home row mods disparando durante
rolling" e recomenda uma combinação de três mecanismos (Pascal
Getreuer/ZSA — ["Introducing Chordal
Hold"](https://blog.zsa.io/chordal-hold/); docs oficiais em
[docs.qmk.fm/tap_hold](https://docs.qmk.fm/tap_hold)):

1. **Flow Tap** (`FLOW_TAP_TERM`) — se a tecla anterior foi digitada há pouco
   tempo (streak de digitação rápida), força o mod-tap a resolver como tap,
   independente de handedness. É o mecanismo desenhado especificamente para o
   caso "letra + space rápido" — a própria documentação oficial usa `KC_SPC`
   como exemplo de tecla elegível.
2. **Chordal Hold** (`CHORDAL_HOLD`) — regra de "mãos opostas": um combo só
   pode resolver como *hold* se as duas teclas envolvidas forem de mãos
   opostas. Evolução do Achordion/Bilateral Combinations, hoje nativa no QMK
   core. Em teclados não-split, exige uma tabela `chordal_hold_layout`
   informando a mão de cada tecla física.
3. **Permissive Hold** (`PERMISSIVE_HOLD`) — resolve *hold* assim que a outra
   tecla é pressionada e solta dentro do tapping term, mantendo combos
   legítimos entre mãos opostas responsivos.

Como o usuário sempre usa o espaço **esquerdo** (mesma mão do `A`), o combo
relatado (`A(hold)+Space`) é coberto tanto pelo Flow Tap quanto pela Chordal
Hold — proteção redundante para o caso reportado.

## Abordagem escolhida

Combinar os três mecanismos (Flow Tap + Permissive Hold + Chordal Hold), por
ser a combinação recomendada pela comunidade/mantenedores para exatamente
esse cenário, e por cobrir tanto o caso relatado (letra + space) quanto
outros rolls do mesmo tipo em quaisquer teclas home-row customizadas via
Launch.

Alternativas descartadas:
- Apenas aumentar `TAPPING_TERM`: não ataca a causa raiz (rolling), deixa a
  digitação "grudenta" em todas as teclas, não só nas problemáticas.
- Apenas `FLOW_TAP_TERM`: resolveria o caso relatado com menos código, mas
  fica sem a rede de segurança da Chordal Hold para rolls same-hand fora de
  uma sequência rápida contígua (ex: pausa seguida de duas teclas da mesma
  mão digitadas juntas).

## Arquitetura

Novo keymap, cópia do `via` existente, sem alterar nada nele:

```
keyboards/keychron/k15_pro/ansi_encoder/rgb/keymaps/danilo/
├── keymap.c      → cópia do keymaps/via/keymap.c (mesmos layers MAC_BASE/
│                    MAC_FN/WIN_BASE/WIN_FN, mesmo encoder_map) + tabela
│                    chordal_hold_layout
├── config.h      → novo arquivo, com os defines de tap-hold
└── rules.mk      → cópia do keymaps/via/rules.mk (VIA_ENABLE=yes,
                     ENCODER_MAP_ENABLE=yes)
```

Os home row mods atuais (`MT(MOD_LGUI, KC_A)` etc.) não são hard-coded no
`keymap.c` — continuam vivendo na EEPROM do teclado, gravados dinamicamente
pelo Keychron Launch via protocolo VIA. As melhorias de tap-hold atuam no
nível do firmware, por baixo de qualquer keycode que o Launch atribuir
dinamicamente.

## Configuração (`config.h`)

```c
#pragma once

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
```

`TAPPING_TERM` permanece no default (200ms).

## Chordal Hold — tabela de mãos (`chordal_hold_layout`)

Tabela adicionada ao `keymap.c`, usando a mesma macro `LAYOUT_90_ansi` já
usada pelos layers, substituindo cada keycode por `'L'`, `'R'` ou `'*'`
(curinga/neutro). Regras de classificação:

- **Linhas alfabéticas (Tab/QWERTY, ASDF, ZXCV) e thumb row**: divididas na
  fronteira clássica de touch-typing entre as colunas `T/G/B` e `Y/H/N`
  (mesma coluna vertical onde ficam os home row mods GACS).
- **Space esquerdo** = `'L'`, **space direito** = `'R'`, `MO(FN)` central =
  `'*'` (neutro).
- **Linha de função (Esc/F-row), linha de números, coluna macro `MC_1`-`MC_5`,
  cluster de navegação (setas/Home/PgUp/PgDn)**: marcados como `'*'`
  (curinga) — não participam de rolls alfabéticos e não há benefício em
  classificá-los por mão, então usar wildcard reduz risco de classificação
  incorreta nessas posições.

A tabela completa (todas as ~90 posições) será construída durante a
implementação copiando literalmente a estrutura de argumentos do
`LAYOUT_90_ansi(...)` já existente no `keymap.c` do keymap `via`, substituindo
cada keycode pelo caractere de mão correspondente — evitando erro de
desalinhamento manual.

## Riscos e mitigação

- **EEPROM/VIA**: flashar firmware novo normalmente não apaga o keymap
  dinâmico salvo via Launch, desde que a estrutura de layers/keycodes do
  `keymap.c` não mude (não muda — é cópia do `via`). Mitigação: exportar um
  backup atualizado do keymap pelo Keychron Launch antes de flashar.
- **Reversibilidade**: se o comportamento ficar estranho (hold "grudento" ou
  tap "vazando" em combos desejados como hold), basta ajustar
  `FLOW_TAP_TERM` ou remover uma das três defines — sem tocar em mais nada.
- **Nada existente é alterado**: `keymaps/via/` original permanece intocado;
  em caso de problema grave, basta recompilar/flashar a `via` original.
- **Compatibilidade com Launch preservada**: `VIA_ENABLE` e
  `ENCODER_MAP_ENABLE` seguem iguais ao keymap `via` original.

## Verificação

1. `qmk compile -kb keychron/k15_pro/ansi_encoder/rgb -km danilo` compila sem
   erro.
2. Flashar e confirmar que o Keychron Launch ainda detecta o teclado e as
   teclas remapeadas anteriormente continuam presentes.
3. Digitar rápido palavras terminando em "a " e variações envolvendo os
   outros home row mods (ex: "da ", "fa ", sequência "asdf") e confirmar que
   **não** aciona modificador indesejado (Spotlight não abre).
4. Segurar `A` deliberadamente (além do tapping term) e apertar Space →
   confirmar que o combo GUI+Space **ainda funciona** (Spotlight abre).
5. Digitar um parágrafo normal e avaliar se a digitação não ficou perceptível
   mais lenta ou "grudenta".

## Fora de escopo

- Não modifica `keymaps/via/` nem qualquer outro arquivo existente do K15
  Pro.
- Não hard-coda os home row mods no firmware — eles continuam configurados
  via Keychron Launch.
- Não cobre o passo a passo de instalação/configuração do QMK CLI nem o
  procedimento de flash (bootloader/DFU) em detalhe — isso fica para o plano
  de implementação.
