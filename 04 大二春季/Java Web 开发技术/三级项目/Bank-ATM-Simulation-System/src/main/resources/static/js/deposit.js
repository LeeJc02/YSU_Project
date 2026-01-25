new Vue({
    el: '#app',
    data: {
        amount: '',
        message: ''
    },
    methods: {
        goBack() {
            window.history.back();
        },
        deposit() {
            if (this.amount <= 0) {
                this.message = '存款金额必须大于0';
                return;
            }
            console.log(typeof this.amount);  // 应该是 'number'
            axios.post('/account/deposit', { amount: this.amount })
                .then(response => {
                    this.message = '存款成功！';
                    // 可以在此更新余额或跳转页面
                })
                .catch(error => {
                    this.message = '存款失败，请稍后再试';
                });
        }
    }
});