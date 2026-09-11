/* 
================= Instruções ===============
1) Coloquem o MODO_DEV = true( para testar sem ESP32)
2) Abram Inicio.html
3) Digite no console do Navegador: localStorage.setItem("nfc_tag", "131FB12");
4) Recarrega a pagina, e pronto. tag lida e voce esta em: emprestimos/lista_livros.html

------- COMANDOS IMPORTANTES DE CONSOLE -------
configurar tag:              localStorage.setItem("nfc_tag", "131FB12");
trocar tag:                  localStorage.setItem("nfc_tag", "OUTRA_TAG_123");
apagar tag:                  localStorage.removeItem("nfc_tag");
apagar tudo de LocalStorage: localStorage.clear();

AVISO!
Atenção aos caminhos
Quando você roda com Live Server, o caminho correto geralmente será: " ./ " e nao mais "../"
==============================================

const MODO_DEV = false; 
function modoDev(){
    if (MODO_DEV) {
        const tagSimulada = localStorage.getItem("nfc_tag");
        if (tagSimulada) {
            console.log("Modo DEV ativo — usando tag:", tagSimulada);
            verificarUsuario(tagSimulada)
            return; // não faz polling
        }
        console.warn("Modo DEV ativo, mas nenhuma tag simulada no localStorage.");
        return;
    }
}
 */


// CAMINHO1: DIGITANDO O --- CODIGO ---
document.getElementById("entrar").addEventListener("click", () => {
    var carteirinha = document.getElementById("codigoInput").value.trim();
    verificarUsuario(carteirinha); 
});

/* CAMINHO2: PROCURANDO O --- ESP32 ---*/
async function localizarRFID() {
    const res = await fetch("/rfid");
    if (res.status === 200) {
        const data = await res.json();
        const tag = data.tag;
        console.log("TAG recebida do ESP32(usuario):", tag);
        verificarUsuario(tag)
    }
    setTimeout(localizarRFID, 500);
}

//---- VERIFICAR USUARIO CADASTRADO (salva dados: "nfc_tag" e "usuario") ---- 
async function verificarUsuario(tag) {   
    const res = await fetch("../banco/alunos.json"); 
    const data = await res.json();

    const usuario = data[tag];

    if (usuario) {
        localStorage.setItem("usuario", JSON.stringify(usuario));

        window.location.href = "../emprestimos/lista_livros.html";
    } else {
        alert("Usuário não encontrado");
    }
}

localizarRFID(); // Inicia o monitoramento