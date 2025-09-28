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
  [*Matrícula*], "539628", "537606", "538606"
)

// apply style
#outline()
#pagebreak()

= Dificuldades encontradas e soluções aplicadas

== Linguagem C <language_c>
A principal dificuldade encontrada foi o domínio da linguagem `C`.

Isso ficou batante evidente quando tentamos lidar com array de strings ou implementar certos conceitos.

Dessa forma, realizamos pesquisas e práticas para entender os problemas.

Além disso, o livro do professor Carlos Maziero e documento de especificação da prática nos auxiliou muito no decorrer do trabalho.

Portanto, muitas vezes sabíamos o que precisava ser feito, mas, uma vez que não tínhamos domínio da linguagem escolhida,
não conseguiamos implementar de uma maneira satisfatória.

Por fim, a implementação de testes automatizados paras funções utilizadas na `main()` também foram um desafio.
Após algumas pesquisas, optamos por utilizar a biblioteca check, podendo ver na @auto_test

== Manipulação de strings
Lidar com string foi, de fato, um problema que enfrentamos inicialmente, principalmente devido a comparações de tipos e ponteiros 
para memória. Contudo, como apontado anteriormente em @language_c, conseguimos solucionar após algumas pesquisas.

Exemplo de problemas enfrentados:
- realizar um split da string(char \*) e salvar no array
- comparar strings(variável do tipo char \* com "string")

```c
args[i] = strtok(input, delim);
while (args[i] != NULL)
  args[++i] = strtok(NULL, delim);
```


== Memoria entre processos.
No trabalho de lidar com as variáveis globais last_child_pid e bg_processes, inicialmente os valores não 
estavam sendo armazenados corretamente. Por esse motivo, ao executar os comandos jobs ou pid, a saída não correspondia ao esperado.

A solução aplicada foi baseada no documento de especificação. A partir dele, conseguimos compreender o funcionamento esperado 
e ajustar a implementação para que os valores das variáveis fossem armazenados corretamente

A princípio, optamos por utilizar a chamada de sistema execve(). No entanto, não sabíamos que 
ela substitui o processo atual pelo novo programa. Ou seja, o processo filho deixa de ser o mesmo após a execução do 
execve() e passa a ser inteiramente o programa chamado. Como consequência, o processo filho perde todas 
as variáveis e estados que possuía antes da chamada.

Isso gerou um grande problema, já que precisávamos manter a lista de PIDs atualizada.
Como o processo filho perde todas as variáveis e estados após a chamada do execve(), 
tornou-se impossível preservar essa lista dentro dele.


= Testes realizados e resultados obtidos

Utilizamos os inputs presentes no documento de especificação da prática, além de testes adicionais criados pela 
equipe através da biblioteca de teste Check.

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
As chamadas de sistema `fork()` e `exec()` foram utilizadas, respectivamente, com o objetivo de criar um novo 
processo e substituir o espaço de memória do processo filho com um novo programa. O `wait()` foi 
empregado para que o processo pai aguardasse a conclusão do processo filho antes de continuar sua execução, 
garantindo, assim, a sincronização entre os processos.

- Processos filhos:
A utilização de uma lista de PID para gerenciar os processos filhos em background foi importante 
para o controle e monitoramento dos processos em execução, evitando a criação de processos zumbis e 
garantindo que todos os processos filhos sejam devidamente aguardados e finalizados.

- Parsing e interpretação de comandos:
A utilização da API strtok e execvp para dividir a string de entrada e interpretar o comando foi 
essencial para a funcionalidade do minishell, permitindo que o shell entenda e execute os comandos fornecidos pelo usuário de forma eficiente.


== Sobre as questoes de reflexão
=== Como os Process IDs (PIDs) diferem entre processos pai e filho?
Ao analisar o comportamento após executar comandos em background no minishell, foi observado 
que o PID do filho é diferente do PID do pai e, além disso, o PID do filho é maior que o do pai. 
Dessa forma, é possível deduzir que um processo veio depois do outro apenas comparando os PIDs

=== Por que as variáveis têm valores diferentes nos dois processos em fork-print.c?
Como se tratam de processos diferentes, eles funcionam como instâncias distintas do mesmo programa. 
Por isso, o sistema operacional aloca áreas de memória separadas para cada um. Dessa forma, após 
o fork(), qualquer alteração feita em variáveis passa a existir apenas no processo que a realizou.

=== O que acontece com o processo filho após a chamada execve()?
O processo filho entra em estado de execução do novo programa, ou seja, ele deixa de ser 
o mesmo que era antes do `execve()` e passa a ser o programa que foi chamado pelo `execve()`. 
Assim, o processo filho perde todas as variáveis e estados que tinha antes do `execve()`.

Enquanto se executado com `execvp()`, o processo filho continua sendo o mesmo, 
apenas executando um novo programa, mas mantendo suas variáveis e estados.
