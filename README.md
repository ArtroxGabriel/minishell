# os-c-template — Template em C para a disciplina de Sistemas Operacionais

Este repositório é um template para acelerar a criação de projetos em C para a sua disciplina de Sistemas Operacionais. Ele já inclui:

- Estrutura de pastas sugerida
- CMake configurado para Linux, macOS e Windows (MSVC)
- Exemplo de biblioteca (`mylib`) e um executável de exemplo (`os_app`)
- Framework mínimo de testes (CTest) e targets auxiliares em CMake
- Configurações para formatação (`.clang-format`) e targets de formatação/lint no `CMakeLists.txt`
- `devcontainer` (VS Code) e imagens Docker + `docker-compose` para desenvolvimento e CI
- Documentação com boas práticas e instruções de uso

Use este template como base — adapte arquivos e convenções conforme as regras da disciplina.

Última atualização: consulte o histórico do repositório.

---

Sumário

- Visão geral
- Estrutura de pastas
- Como compilar (Linux / macOS / Windows)
- Testes
- Formatação e lint
- Sanitizers e cobertura
- Instalação de dependências C populares
- Devcontainer / Docker / docker-compose
- Boas práticas e recomendações
- Como contribuir

---

Visão geral
Você deve:

- Escrever código em C (padrão C11 por padrão do template).
- Manter uma API pública em `include/`.
- Implementações em `src/`.
- Testes em `tests/`.
- Usar CMake como sistema de build portátil.

Estrutura de pastas (padrão do template)

- `CMakeLists.txt` — top-level (configura flags, sanitizers, targets `format` e `lint`, exporta compile_commands)
- `src/` — código-fonte (.c) e `src/CMakeLists.txt` que cria `mylib` e `os_app`
- `include/` — cabeçalhos públicos (.h)
- `tests/` — testes (CTest)
- `.clang-format` — regras de formatação
- `.devcontainer/` — configuração para VS Code devcontainer
- `docker/` — Dockerfile e docker-compose para builds/testes em container
- `tools/` — scripts utilitários (opcional)
- `docs/` — documentação adicional

---

Como compilar

Pré-requisitos (exemplos):

- Linux (Ubuntu): `build-essential`, `cmake`, `ninja-build` (opcionais: `clang`, `clang-format`, `clang-tidy`, `valgrind`, `lcov`)
- macOS: `brew install cmake ninja` (recomendo instalar `clang-format` e `clang-tidy` via brew)
- Windows: Instale Visual Studio (com Desktop development with C++), ou use `mingw` + CMake. No Windows com MSVC, o projeto detecta o compilador automaticamente.

Passos básicos (cross-platform):

1. Criar diretório de build:
   - `cmake -S . -B build -G "Ninja"`
2. Compilar:
   - `cmake --build build --config Debug`
3. Executar (exemplo):
   - Linux/macOS: `./build/os_app` (ou `./build/os_app` dependendo do generator/output)
   - Windows (MSVC): `.\build\Debug\os_app.exe`

Observações:

- O `CMakeLists.txt` do template exporta `compile_commands.json` (útil para clangd/clang-tidy/editores).
- Para build com sanitizers habilitados: passar `-DENABLE_SANITIZERS=ON` ao configurar o CMake.

Exemplo completo:

- `cmake -S . -B build -G Ninja -DENABLE_SANITIZERS=ON`
- `cmake --build build`

---

Testes

- O template usa CTest. Para construir e executar os testes:
  - `cmake -S . -B build`
  - `cmake --build build --target test` (ou `ctest --test-dir build --output-on-failure`)
- Os testes ficam em `tests/` e o CMake gerará um executável de teste `test_mylib`.
- Para integrar frameworks como `cmocka`, instale a lib no sistema (ex.: `libcmocka-dev` no Debian/Ubuntu). O `tests/CMakeLists.txt` detecta `cmocka` e usa quando disponível.

---

Formatação e Lint

Formatador

- O projeto fornece `.clang-format`. Para aplicar:
  - Localmente: instale `clang-format` e rode `clang-format -i <arquivos.c> <arquivos.h>`
  - Com CMake: existe o target `format` (se `clang-format` estiver instalado): `cmake --build build --target format`
- Configurações principais estão em `.clang-format`. Ajuste o estilo conforme necessário.

Lint / Static analysis

- `clang-tidy` está configurado como target `lint` quando disponível; requer `compile_commands.json`.
  - Exemplo: `cmake --build build --target lint`
- `cppcheck` é um analizador estático alternativo (não integrado por padrão):
  - `cppcheck --enable=all --inconclusive --std=c11 --language=c src include`
- O devcontainer instala `clang-format`, `clang-tidy`, `cppcheck` e `valgrind` por conveniência.

Recomendações:

- Execute o `format` antes de abrir PRs.
- Execute `lint` e corrija avisos significativos (priorize erros que possam indicar UB).

---

Sanitizers e análise dinâmica

- Ative sanitizers via CMake: `-DENABLE_SANITIZERS=ON` (somente em GCC/Clang; MSVC tem opções distintas).
- Sanitizers úteis:
  - AddressSanitizer (ASAN): detecta use-after-free, buffer-overflow
  - UndefinedBehaviorSanitizer (UBSAN): detecta comportamentos indefinidos
- Exemplo:
  - `cmake -S . -B build -G Ninja -DENABLE_SANITIZERS=ON`
  - `cmake --build build`
  - `ASAN_OPTIONS=detect_leaks=1 ./build/src/os_app`
- Para análise de memória: use `valgrind` se estiver usando GCC (note que ASAN e Valgrind normalmente não são usados juntos).

Cobertura de testes (coverage)

- Para cobertura com GCC/Clang: habilite flags de coverage no CMake (o template tem opção `ENABLE_CODE_COVERAGE`):
  - `cmake -S . -B build -DENABLE_CODE_COVERAGE=ON`
  - `cmake --build build`
  - `ctest --test-dir build`
  - Gere relatórios com `lcov`/`genhtml` ou `gcovr`.
- A opção de cobertura é sensível ao compilador; verifique suporte antes de habilitar.

---

Instalando pacotes C (dependências)

- Linux (Debian/Ubuntu):
  - `sudo apt-get update && sudo apt-get install -y build-essential cmake ninja-build libcmocka-dev`
- macOS (Homebrew):
  - `brew install cmake ninja cmocka clang-format clang-tidy`
- Windows:
  - Use vcpkg (`https://github.com/microsoft/vcpkg`) ou `choco`/pacotes do Visual Studio. Exemplo com vcpkg:
    - `git clone https://github.com/microsoft/vcpkg.git && ./vcpkg/bootstrap-vcpkg.sh`
    - `./vcpkg install cmocka`
    - Integre com CMake via toolchain do vcpkg.
- Gerenciadores de pacotes C/C++:
  - vcpkg — multiplataforma, bom para bibliotecas C/C++.
  - Conan — sistema de pacotes flexível com ampla adoção.

Adicionando dependências no CMake:

- Prefira `find_package()` sempre que possível.
- Para projetos que usam vcpkg, use o toolchain file do vcpkg ao configurar CMake:
  - `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg.cmake`

---

Devcontainer (VS Code)

- Pasta: `.devcontainer/`
- Para abrir no VS Code: use "Reopen in Container" (requer a extensão Remote - Containers).
- O devcontainer instala clangd, clang-format, clang-tidy, cmake e outras ferramentas.
- Configurações principais:
  - `cmake.configureOnOpen = true` e `clangd` configurado para usar `compile_commands.json`.

Docker / docker-compose

- Imagens e compose em `docker/`.
- Uso rápido:
  - Build da imagem: `docker build -t os-c-template:dev -f docker/Dockerfile .`
  - Rodar container interativo (montando o código):
    - `docker run --rm -it -v "$(pwd)":/workspace -w /workspace os-c-template:dev`
  - Orquestração com docker-compose:
    - `docker compose -f docker/docker-compose.yml up --build dev`
- O Dockerfile contém um entrypoint que facilita `build` e `test`.

---

Boas práticas e recomendações

- Organização:
  - Separe API pública (`include/`) da implementação (`src/`).
  - Evite grandes arquivos monolíticos; prefira módulos pequenos e testáveis.
- Build e CI:
  - Mantenha builds determinísticos (specifique padrão C e flags mínimas).
  - Gere `compile_commands.json` para análise estática contínua.
- Segurança e robustez:
  - Verifique retornos de alocação (`malloc`) e use tipos fixos (`<stdint.h>`).
  - Evite UB (considere compiladores com flags estritas `-Wall -Wextra -Wpedantic`).
- Code style:
  - Use `.clang-format` e aplique automaticamente (pre-commit hook recomendado).
- Revisão e testes:
  - Espera-se cobertura mínima de testes; escreva testes unitários para lógicas críticas.
  - Use sanitizers e valgrind regularmente.
- Portabilidade:
  - Teste em Linux/macOS/Windows quando possível.
  - Encapsule diferenças de plataforma em pequenos módulos (ex.: wrappers posix/Win32).

Exemplo de pre-commit simples (sugestão)

- Instale `pre-commit` e configure um hook para `clang-format`:
  - `python3 -m pip install pre-commit`
  - Adicione um `.pre-commit-config.yaml` que execute `clang-format` e `cppcheck`.

---

Sugestões para CI

- Stages recomendadas:
  - style-check: rodar `clang-format` (dif) e `clang-tidy`
  - build: compilar em Release/Debug
  - test: executar `ctest`
  - sanitizers: compilar com `-DENABLE_SANITIZERS=ON` e executar testes
  - coverage: coletar cobertura com `lcov/gcovr`
- Use a imagem Docker do projeto (`docker/Dockerfile`) para garantir ambiente consistente.

---

Como contribuir

- Mantenha commits pequenos e atômicos.
- Abra PRs com descrição do objetivo e testes/fixes incluídos.
- Execute `cmake --build build --target format` e `cmake --build build --target lint` (quando disponível) antes de submeter PR.
- Atualize `README.md` e `CHANGELOG` quando necessário.

---

Arquivos importantes

- `CMakeLists.txt` (top-level) — opções de build, sanitizers, format/lint targets
- `src/` — código fonte e `src/CMakeLists.txt`
- `include/` — cabeçalhos públicos
- `tests/` — testes e `tests/CMakeLists.txt`
- `.clang-format` — estilo
- `.devcontainer/` — devcontainer config
- `docker/` — Dockerfile e docker-compose

---

Contato e suporte

- Use o sistema de issues do repositório para reportar bugs ou pedir melhorias no template.
- Para dúvidas sobre CMake ou portabilidade, inclua detalhes do sistema (OS, versão do compilador, comandos executados e saídas de erro).

---

Notas finais

- Este template é um ponto de partida: adapte o estilo, ferramentas e fluxo de CI conforme as necessidades da disciplina e políticas da sua instituição.
- Mantenha a documentação atualizada para que novos alunos consigam clonar o template e começar rapidamente.

Boa sorte e bom desenvolvimento!
