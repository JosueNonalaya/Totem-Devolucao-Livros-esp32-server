document.addEventListener("DOMContentLoaded", () => {
    const titulo = sessionStorage.getItem("tituloLivroParaDevolver");

    if (!titulo) {
        document.getElementById("tituloLivro").textContent = "(desconhecido)";
        return;
    }

    document.getElementById("tituloLivro").textContent = `"${titulo}"`;
});

document.getElementById("finalizar").addEventListener("click", (event) => {
    localStorage.clear(); // limpa todos os dados da sessão
    window.location.href = "../inicio.html";
});

document.getElementById("devolver").addEventListener("click", (event) => {
    window.location.href = "../emprestimos/lista_livros.html";
});