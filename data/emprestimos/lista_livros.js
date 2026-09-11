document.addEventListener("DOMContentLoaded", () => {
    usuario = JSON.parse(localStorage.getItem("usuario"));
    criarLista(usuario);
});

document.addEventListener("click", function (e) {
    if (e.target.classList.contains("return-button")) {
        const idLivro = e.target.dataset.livroId;
        devolverLivro(idLivro, e.target);
    }
});

// CRIA LISTA DE LIVROS
async function criarLista(usuario){
    const response = await fetch("../banco/livros.json");
    const livrosJson = await response.json();
    const container = document.querySelector(".books-container");
    
    const espaco_nome = document.querySelector(".user-name");
    espaco_nome.textContent = usuario.nome;


    container.innerHTML = ""; // evita duplicação visual

    usuario.livros.forEach(idLivro => {
        const livro = livrosJson[idLivro];
        if (!livro) return;

        const div = document.createElement("div");
        div.classList.add("book-item");

        div.innerHTML = `
            <div class="book-cover"></div>

            <div class="book-info">
                <div class="book-title">${livro.titulo}</div>
                <div class="book-due-date">
                    <span class="due-date-icon"></span>
                    Vence em: ${livro.vencimento ?? "Sem data"}
                </div>
            </div>

            <button class="return-button" data-livro-id="${idLivro}">Devolver</button>
        `;

        container.appendChild(div);
    });
}

// BOTAO DE DEVOLVER LIVRO
function devolverLivro(idLivro, button) {
    try {
        const usuario = JSON.parse(localStorage.getItem("usuario"));

        if (!usuario || !Array.isArray(usuario.livros)) {
            throw new Error("Usuário inválido ou sem lista de livros.");
        }

        // 1. Guarda o livro que será devolvido
        sessionStorage.setItem("idLivroParaDevolver", idLivro);
        sessionStorage.setItem("tituloLivroParaDevolver", idLivro.titulo);

        // 2. Redireciona para a tela intermediária
        window.location.href = "../lendolivro/lerlivro.html";
        
    } catch (erro) {
        console.error("Erro ao preparar devolução:", erro);
        window.location.href = "../retorno/livro_nao_devolvido.html";
    }
}

// BOTAO FINALIZAR SESSAO
function finalizarSessao() {
  sessionStorage.clear();
  window.location.href = "../inicio.html";
}