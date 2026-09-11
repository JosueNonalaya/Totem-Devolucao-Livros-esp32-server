document.addEventListener("DOMContentLoaded", () => {
    const idLivro = sessionStorage.getItem("idLivroParaDevolver");
    if (!idLivro) {
        alert("Nenhum livro selecionado!");
        window.location.href = "../emprestimos/lista_livros.html";
        return;
    }

    carregarTituloLivro(idLivro);

    iniciarLeituraLivro(idLivro);
});

document.getElementById("cancelar").addEventListener("click", () => {
    window.location.href = "../emprestimos/lista_livros.html";
});

async function carregarTituloLivro(idLivro) {
    try {
        const res = await fetch("../banco/livros.json");
        const livros = await res.json();

        const livro = livros[idLivro];

        if (!livro) {
            document.querySelector("#livroSelecionado").textContent = "Livro não encontrado";
            return;
        }

        document.querySelector("#livroSelecionado").textContent = livro.titulo;
        sessionStorage.setItem("tituloLivroParaDevolver", livro.titulo);

    } catch (erro) {
        console.error("Erro ao carregar título do livro:", erro);
        document.querySelector("#livroSelecionado").textContent = "Erro ao carregar título";
    }
}


async function iniciarLeituraLivro(idLivro) {

    console.log("Aguardando leitura do RFID para livro:", idLivro);

    let tempoMaximo = 15000;  // 15 segundos
    let tempoInicio = Date.now();

    async function verificarLeitura() {

        // ⏳ Se passou 15s sem ler nada → volta para lista
        if (Date.now() - tempoInicio >= tempoMaximo) {
            console.warn("⏳ Tempo expirou — nenhuma leitura detectada!");
            window.location.href = "../emprestimos/lista_livros.html";
            return;
        }

        const res = await fetch("/rfid");  // ESP32

        if (res.status === 200) {
            const data = await res.json();
            const tagLida = data.tag;

            console.log("TAG lida:", tagLida);

            if (tagLida === idLivro) {
                console.log("Livro CONFIRMADO. Enviando devolução...");

                processarDevolucaoLocal(idLivro);

                await fetch("/devolver?id=" + idLivro);
                window.location.href = "../retorno/livro_devolvido.html";
                return;

            } else {
                console.warn("TAG não corresponde ao livro!");
                window.location.href = "../retorno/livro_nao_devolvido.html";
                return;
            }
        }

        // Continua verificando a cada 400 ms
        setTimeout(verificarLeitura, 400);
    }

    verificarLeitura();
}

// REMOVE DO LOCAL STORAGE
function processarDevolucaoLocal(idLivro) {
    let usuario = JSON.parse(localStorage.getItem("usuario"));

    if (!usuario || !Array.isArray(usuario.livros)) {
        console.error("Usuário inválido no LocalStorage");
        return;
    }

    usuario.livros = usuario.livros.filter(l => l !== idLivro);

    localStorage.setItem("usuario", JSON.stringify(usuario));

    console.log("Livro removido localmente:", idLivro);
}
