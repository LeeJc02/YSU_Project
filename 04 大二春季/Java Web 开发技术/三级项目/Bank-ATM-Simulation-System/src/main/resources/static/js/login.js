new Vue({
    el: '#app',
    data: () => ({
        login: {
            cardNumber: '',
            password: ''
        }
    }),
    methods: {
        // 返回上一页
        goBack() {
            window.history.back();
        },

        // 用户登录逻辑
        async loginUser() {
            try {
                const response = await axios.post('/login', {
                    cardNumber: this.login.cardNumber,
                    password: this.login.password
                });
                console.log(response);
                if (response.status == 200) {
                    localStorage.setItem('balance', response.data.balance);
                    window.location.href = '/account/menu';
                } else {
                    this.showError('登录失败，请检查凭证信息');
                }
            } catch (error) {
                console.log(error)
                this.showError('系统异常，请稍后重试');
            }
        },

        // 显示错误提示
        showError(message) {
            alert(message);
            this.login.password = '';
        }
    }
});
