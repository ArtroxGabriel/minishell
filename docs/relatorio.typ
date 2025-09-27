#set raw(theme: "./themes/dracula.mtTheme")

#set text(region: "BR", lang: "pt")
#set heading(numbering: "1.a.i)")

#show link: set text(fill: blue)
#show raw.where(block: false): box.with(
  fill: luma(240),
  inset: (x: 3pt, y: 0pt),
  outset: (y: 3pt),
  radius: 2pt,
)
#show raw.where(block:true): it => block(
  fill: rgb("#1d2433"),
  inset: 8pt,
  radius: 5pt,
  text(fill: white, it)
)

#text(2em)[Atividade Prática: Mini-Shell]

#table(
  columns: (auto, 1fr, 1fr, 1fr),
  [*Nome*],  "Antonio Gabriel", "Erik Bayerlein", "Julia Naomi",
  [*Matrícula*], "539628", "<define>", "538606"
)

// apply style
#outline()
#pagebreak()

= Dificuldades encontradas e soluções aplicadas

== Linguagem C <language_c>
Os membros da equipe tiveram uma dificuldade com `C` como por exemplo lidar com array de strings ou como implementar certos conceitos.

A solução aplicada foi pesquisa e muita prática, evitamos o uso de LLM's para resolver problemas de código, nos forçando a pesquisar e entender o problema, por exemplo, sabíamos o que precisávamos fazer mas não como fazer em C, por isso pesquisamos como fazer e testamos implementação, além disso o documento de especificação da pratica auxiliou muito.

Semelhante a isso, implementar testes automatizados paras funções utilizadas na `main()`, também foram um desafio, por recomendação de um amigo, optamos por utilizar a biblioteca check. Apos algumas pesquisas de como configurar e implementar testes automatizados em C, os testes foram implementados com sucesso, como se pode ver na @auto_test

== Manipulação de strings
Lidar com string foi o pequeno problema que enfrentamos inicialmente devido a comparações de tipos e ponteiros para memória, mas como dito no @language_c, após algumas pesquisas em sites como #link("https://stackoverflow.com")[stackoverflow]

Um dos exemplos enfrentados foram:
- realizar um split da string(char \*) e salvar no array
- comparar strings(variável do tipo char \* com "string")

Novamente, como dito na @language_c, a solução aplicada foi soluções encontradas no stackoverflow com sugestões de como implementar, por exemplo, "how to split string in C", "how to split string and save in an array".

```c
args[i] = strtok(input, delim);
while (args[i] != NULL)
  args[++i] = strtok(NULL, delim);
```


== Memoria entre processos.

Na parte do trabalho de lidar com as variáveis globais, last_child_pid e bg_processes, a principio o valor não estava sendo salvo corretamente, por isso quando executava o comando jobs ou pid, o output não estava como deveria.

A solução aplicada veio do documento de especificação, através dele conseguimos entender

A principio escolhemos utilizar a metodologia do execve mas o que não sabíamos é que o execve substitui o processo atual pelo novo processo, ou seja, o processo filho deixa de ser o mesmo que era antes do execve() e passa a ser o programa que foi chamado pelo execve(), por conta disso, o processo filho perde todas as variáveis e estados que tinha antes do execve().
Isso nos trouxe um grande problema, pois precisamos manter a lista de pids atualizada, e como o processo filho perde todas as variáveis e estados que tinha antes do execve(), não conseguimos manter a lista de pids atualizada.


= Testes realizados e resultados obtidos

Para testar o minishell foi utilizado os input presentes no documento de especificação da prática, além de testes adicionais criados pela equipe.
utilizando a biblioteca de teste Check.

== Teste manuais:

Na @basic_test testes para função de comandos internos e externos mas sem lidar com subprocessos.
Já na @advanced_test foi testado os comandos internos e o comportamento ao lidar com subprocessos e funções padrões do POSIX como wait e jobs.

=== Testes basicos: <basic_test>
#figure(
  image("images/testes_basicos.png", width: 70%),
  caption: "execução dos testes básicos"
)

=== Testes avançados <advanced_test>

#figure(
  image("images/testes_avancados.png", width: 70%),
  caption: "execução dos testes avançados"
)

#pagebreak()

=== Testes automatizados: <auto_test>

#figure(
  image("images/testes_automatizados.png"),
  caption: [execução dos testes automatizados]
)

#figure(

```c
void setup(void) {
  bg_count = 0;
  memset(bg_processes, 0, sizeof(bg_processes));
}

START_TEST(test_parse_command_basic) {
  char input[] = "ls -l";
  char *args[MAX_ARGS];
  int background = 0;
  parse_command(input, args, &background);

  ck_assert_str_eq(args[0], "ls");
  ck_assert_str_eq(args[1], "-l");
  ck_assert_ptr_eq(args[2], NULL);
  ck_assert_int_eq(background, 0);
}
END_TEST

{...}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = minishell_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : 1;
}

```,
caption: [trecho do código de testes]
)

#pagebreak()
= Análise dos conceitos de SO aplicados

Conceitos aplicados:
- processos em background e foreground:
as chamadas de sistema `fork()` e `exec()` que foram utilizadas, respectivamente, para criar um novo processo e para substituir o espaço de memória do processo filho com um novo programa. o `wait()` foi utilizado para que o processo pai aguardasse a conclusão do processo filho antes de continuar sua execução, garantindo a sincronização entre os processos.
- Processos filhos:
A utilização de uma lista de PID para gerencias os processos filhos em background foi importante para o controle e monitoramento dos processos em execução, evitando a criação de processos zumbis, garantindo que todos os processos filhos sejam devidamente aguardados e finalizados.
- Parsing e interpretação de comandos:
a uso da API strtok e execvp para dividir a string de entrada e interpretar o comando foi essencial para a funcionalidade do minishell, permitindo que o shell entenda e execute os comandos fornecidos pelo usuário de forma eficiente.


== Sobre as questoes de reflexão
=== Como os Process IDs (PIDs) diferem entre processos pai e filho?
Ao analisar o comportamento apos executar comandos em background no minishell, foi observado que o PID do filho é diferente do PID do pai e além disso, o PID do filho é maior que o do pai, o que é possível deduzir que um processo veio depois do outro apenas comparando os PIDs

=== Por que as variáveis têm valores diferentes nos dois processos em fork-print.c?
Como são processos diferentes, são como instancias diferente do mesmo programa, por conta disso, o S.O. aloca espaços diferentes de memoria para cada um, ao fazer isso, os valores das variáveis, caso sejam alteradas apos o `fork()`, terão valores diferente em cada processo

=== O que acontece com o processo filho após a chamada execve()?
O processo filho entra em estado de execução do novo programa, ou seja, o processo filho deixa de ser o mesmo que era antes do `execve()` e passa a ser o programa que foi chamado pelo `execve()`, por conta disso, o processo filho perde todas as variáveis e estados que tinha antes do `execve()`.

Enquanto se executado com `execvp()`, o processo filho continua sendo o mesmo, apenas executando um novo programa, mas mantendo suas variáveis e estados.
